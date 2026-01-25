// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "ParryDetectorComponent.h"
#include "SquadManagerComponent.h"
#include "TargetingComponent.h"
#include "ChainAttackComponent.h"
#include "Taggable.h"
#include "Dazeable.h"

#include "GameFramework/GameModeBase.h"

AZGPPlayerController::AZGPPlayerController()
{
	// Components
	m_pSquadManagerComponent = CreateDefaultSubobject<USquadManagerComponent>(TEXT("SquadManagerComponent"));
	m_pTargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	m_pParryDetectorComponent = CreateDefaultSubobject<UParryDetectorComponent>(TEXT("ParryDetectorComponent"));
	m_pChainAttackComponent = CreateDefaultSubobject<UChainAttackComponent>(TEXT("ChainAttackComponent"));

	// Input Context
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-move.ia-move"));
	if (MoveActionAsset.Succeeded())
	{
		IA_Move = MoveActionAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-look.ia-look"));
	if (LookActionAsset.Succeeded())
	{
		IA_Look = LookActionAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-jump.ia-jump"));
	if (JumpActionAsset.Succeeded())
	{
		IA_Jump = JumpActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> AttackActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-attack.ia-attack"));
	if (AttackActionAsset.Succeeded())
	{
		IA_Attack = AttackActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> TagActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-tag.ia-tag"));
	if (TagActionAsset.Succeeded())
	{
		IA_Tag = TagActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LockOnActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-lockOn.ia-lockOn"));
	if (LockOnActionAsset.Succeeded())
	{
		IA_LockOn = LockOnActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> DodgeAsset(TEXT("/Game/ZGProject/ZGPInput/ia-dodge.ia-dodge"));
	if (DodgeAsset.Succeeded())
	{
		IA_Dodge = DodgeAsset.Object;
	}

	// Input Mapping Context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextAsset(TEXT("/Game/ZGProject/ZGPInput/imc-default.imc-default"));
	if (DefaultMappingContextAsset.Succeeded())
	{
		IMC_Default = DefaultMappingContextAsset.Object;
	}
}

void AZGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (IMC_Default)
		{
			Subsystem->AddMappingContext(IMC_Default, DefaultInputMappingPriority);
		}
	}

	if (m_pSquadManagerComponent == nullptr) return;

	if (m_pSquadManagerComponent)
	{
		m_pSquadManagerComponent->OnParryTagExecute.AddDynamic(this, &AZGPPlayerController::HandleParryTag);

		m_pSquadManagerComponent->InitializeSquad();

		APawn* FirstCharacter = m_pSquadManagerComponent->GetFirstCharacter();
		if (FirstCharacter)
		{
			Possess(FirstCharacter);
		}
	}

	if (m_pChainAttackComponent)
	{
		m_pChainAttackComponent->OnChainAttackExecuted.AddDynamic(this, &AZGPPlayerController::HandleChainAttackExecute);
		m_pChainAttackComponent->OnChainAttackFinished.AddDynamic(this, &AZGPPlayerController::HandleChainAttackFinish);
		m_pChainAttackComponent->OnChainAttackCancelled.AddDynamic(this, &AZGPPlayerController::HandleChainAttackCancel);
	}

	// 타겟 변경 시 Character에 전달
	if (m_pTargetingComponent)
	{
		m_pTargetingComponent->OnTargetChanged.AddDynamic(this, &AZGPPlayerController::HandleTargetChange);
	}
}

void AZGPPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_pTargetingComponent)
	{
		APawn* ControlPawn = GetPawn();
		if (ControlPawn)
		{
			const FVector Location = ControlPawn->GetActorLocation();
			// ControlPawn의 Forward가 아닌 플레이어가 보는 방향인 카메라의 Forward 사용하는 것 고려
			FVector Forward = ControlPawn->GetActorForwardVector();
			if (PlayerCameraManager)
			{
				Forward = PlayerCameraManager->GetCameraRotation().Vector();
			}

			m_pTargetingComponent->UpdateTargeting(DeltaTime, Location, Forward, ControlPawn);
		}
	}

}

void AZGPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 새로 빙의한 캐릭터가 PlayerCharacter인지 확인
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnChainAttackSkillFinish.AddDynamic(this, &AZGPPlayerController::HandleChainAttackSkillFinish);
	}
}

void AZGPPlayerController::OnUnPossess()
{
	APawn* InPawn = GetPawn();

	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnChainAttackSkillFinish.RemoveDynamic(this, &AZGPPlayerController::HandleChainAttackSkillFinish);
	}

	Super::OnUnPossess();
}

void AZGPPlayerController::HandleChainAttackSkillFinish()
{
	// 체인 어택 상태일 때만 로직 수행
	if (m_pChainAttackComponent && m_pChainAttackComponent->IsChainExecuteing())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Controller] Chain Attack Skill Finished -> FinishChainAttack()"));
		m_pChainAttackComponent->FinishChainAttack();
	}
}

void AZGPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Move 바인딩
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZGPPlayerController::HandleMove);
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &AZGPPlayerController::HandleStopMove);
		}

		// Look 바인딩
		if (IA_Look)
		{
			EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AZGPPlayerController::HandleLook);
		}

		// Jump 바인딩
		if (IA_Jump)
		{
			EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AZGPPlayerController::HandleJump);
			EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AZGPPlayerController::HandleStopJumping);
		}

		// Attack 바인딩
		if (IA_Attack)
		{
			EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AZGPPlayerController::HandleAttack);
		}

		// Tag 바인딩
		if (IA_Tag)
		{
			EnhancedInputComponent->BindAction(IA_Tag, ETriggerEvent::Started, this, &AZGPPlayerController::HandleTag);
		}

		// LockOn 바인딩
		if (IA_LockOn)
		{
			EnhancedInputComponent->BindAction(IA_LockOn, ETriggerEvent::Started, this, &AZGPPlayerController::HandleLockOn);
		}

		// Dodge 바인딩
		if (IA_Dodge)
		{
			EnhancedInputComponent->BindAction(IA_Dodge, ETriggerEvent::Started, this, &AZGPPlayerController::HandleDodge);
		}
	}
}

AActor* AZGPPlayerController::GetCurrentTargetActor_Implementation()
{
	if (m_pTargetingComponent)
	{
		return m_pTargetingComponent->GetCurrentTarget();
	}
	return nullptr;
}

void AZGPPlayerController::SetCurrentTargetActor_Implementation(AActor* NewTarget)
{
	//if (m_pTargetingComponent)
	//{
	//	m_pTargetingComponent->SetCurrentTarget(NewTarget);
	//}
}

bool AZGPPlayerController::IsTargetLock_Implementation() const
{
	if (m_pTargetingComponent)
	{
		return m_pTargetingComponent->IsHardLock();
	}
	return false;
}

bool AZGPPlayerController::TryTriggerChainAttack_Implementation(AActor* DazedEnemy)
{
	if (!m_pChainAttackComponent) return false;
	if (!DazedEnemy) return false;

	if (m_pChainAttackComponent->IsChainActive()) return false;

	m_pCurrentChainTarget = DazedEnemy;

	PauseDazeTimer(DazedEnemy, true);

	bool bSuccess = m_pChainAttackComponent->TriggerChainAttack(DazedEnemy);

	UE_LOG(LogTemp, Warning, TEXT("[PlayerController] TryTriggerChainAttack on %s - %s"),
		*DazedEnemy->GetName(), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));

	return bSuccess;
}

bool AZGPPlayerController::TryTriggerParryAssist_Implementation(AActor* Attacker, const FVector& AttackDirection)
{
	return false;
}

bool AZGPPlayerController::TryTriggerQuickAssist_Implementation()
{
	return false;
}

void AZGPPlayerController::SelectChainAttackCharacter(int32 SelectedIndex)
{
	if (!m_pChainAttackComponent) return;
	if (!m_pChainAttackComponent->IsChainWaitingInput()) return;

	// 체인 어택 실행
	if (m_pChainAttackComponent->ExecuteChainAttack())
	{
		AActor* ChainTarget = m_pChainAttackComponent->GetChainTarget();

		if (m_pSquadManagerComponent && ChainTarget)
		{
			m_pSquadManagerComponent->RequestChainAttack(SelectedIndex, ChainTarget);
		}
	}
}

void AZGPPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Move(Value);
	}
}

void AZGPPlayerController::HandleStopMove()
{
	if (APlayerCharacter* ControllerCharacter = GetPawn<APlayerCharacter>())
	{
		ControllerCharacter->StopMove();
	}
}

void AZGPPlayerController::HandleLook(const FInputActionValue& Value)
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Look(Value);
	}
}

void AZGPPlayerController::HandleJump()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Jump();
	}
}

void AZGPPlayerController::HandleStopJumping()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->StopJumping();
	}
}

void AZGPPlayerController::HandleDodge()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->RequestDodge();
	}
}

void AZGPPlayerController::HandleParryTag(APawn* NewActiveCharacter, AActor* ParriedEnemy)
{
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(NewActiveCharacter))
	{
		PlayerCharacter->RequestParryAttack(ParriedEnemy);
	}
}

void AZGPPlayerController::HandleChainAttackExecute()
{
}

void AZGPPlayerController::HandleChainAttackCancel()
{
}

void AZGPPlayerController::HandleChainAttackFinish()
{
	if (m_pCurrentChainTarget.IsValid())
	{
		PauseDazeTimer(m_pCurrentChainTarget.Get(), false);
		m_pCurrentChainTarget.Reset();
	}
}

void AZGPPlayerController::HandleAttack()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->RequestAttack();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController : GetPawn<PlayerCharacter> is NULL"));
	}

}

void AZGPPlayerController::HandleTag()
{
	// 체인 어택 우선
	if (m_pChainAttackComponent && m_pChainAttackComponent->IsChainWaitingInput())
	{
		// UI에서 처리하므로 아무것도 하지 않음
		return;
	}

	// 1. 패리 가능하면 패리 우선
	if (m_pParryDetectorComponent)
	{
		bool bCanParry = m_pParryDetectorComponent->CanParry();
		UE_LOG(LogTemp, Warning, TEXT("[HandleTag] CanParry: %s"), bCanParry ? TEXT("TRUE") : TEXT("FALSE"));

		if (bCanParry)
		{
			AActor* ParryTarget = m_pParryDetectorComponent->GetParryTarget();
			UE_LOG(LogTemp, Warning, TEXT("[HandleTag] ParryTarget: %s"),
				ParryTarget ? *ParryTarget->GetName() : TEXT("NULL"));

			if (m_pParryDetectorComponent->ExecuteParry())
			{
				UE_LOG(LogTemp, Warning, TEXT("[HandleTag] ExecuteParry SUCCESS"));

				if (m_pSquadManagerComponent)
				{
					m_pSquadManagerComponent->RequestParryTag(ParryTarget);
				}
				return;
			}
		}
	}

	// 2. 일반 태그
	if (m_pSquadManagerComponent)
	{
		m_pSquadManagerComponent->RequestTag();
	}
}

void AZGPPlayerController::HandleLockOn()
{
	if (m_pTargetingComponent)
	{
		m_pTargetingComponent->ToggleLockOn();

		APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
		if (PlayerCharacter)
		{
			if (m_pTargetingComponent->IsHardLock())
			{
				AActor* Target = m_pTargetingComponent->GetCurrentTarget();
				PlayerCharacter->SetHardLockTarget(Target);
			}
			else
			{
				PlayerCharacter->ClearHardLockTarget();
			}
		}
	}
}

void AZGPPlayerController::HandleTargetChange(AActor* NewTarget)
{
	APlayerCharacter* PlayerCharacter = GetPawn<APlayerCharacter>();
	if (!PlayerCharacter) return;

	if (m_pTargetingComponent && m_pTargetingComponent->IsHardLock())
	{
		if (NewTarget)
		{
			PlayerCharacter->SetHardLockTarget(NewTarget);
		}
		else
		{
			PlayerCharacter->ClearHardLockTarget();
		}
	}
}

void AZGPPlayerController::PauseDazeTimer(AActor* Target, bool bPause)
{
	if (Target && Target->Implements<UDazeable>())
	{
		IDazeable::Execute_PauseDazeTimer(Target, bPause);
	}
}



