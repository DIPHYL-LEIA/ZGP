// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "InputAction.h"

#include "SquadManagerComponent.h"
#include "TargetingComponent.h"
#include "Taggable.h"

#include "GameFramework/GameModeBase.h"

AZGPPlayerController::AZGPPlayerController()
{
	// Components
	m_pSquadManagerComponent = CreateDefaultSubobject<USquadManagerComponent>(TEXT("SquadManagerComponent"));
	m_pTargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));

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

	if (m_pSquadManagerComponent && m_arTagCharacter.Num() > 0)
	{

		APlayerCharacter* FirstCharacter = nullptr;
		FVector CharacterLocation = FVector::ZeroVector;
		FRotator CharacterRotation = FRotator::ZeroRotator;

		// Player Start 위치 
		if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
		{
			AActor* PlayerStart = GameMode->FindPlayerStart(this);
			if (PlayerStart)
			{
				CharacterLocation = PlayerStart->GetActorLocation();
				CharacterRotation = PlayerStart->GetActorRotation();
			}
		}

		if (GetPawn())
		{
			GetPawn()->Destroy();
		}

		for (int i = 0; i < m_arTagCharacter.Num(); ++i)
		{
			if (m_arTagCharacter[i])
			{
				APlayerCharacter* NewCharacter = GetWorld()->SpawnActor<APlayerCharacter>(m_arTagCharacter[i], CharacterLocation, CharacterRotation); //

				if (NewCharacter)
				{
					m_pSquadManagerComponent->RegisterCharacter(NewCharacter);

					if (i == 0)
					{
						FirstCharacter = NewCharacter;
						Possess(FirstCharacter);

						m_pSquadManagerComponent->InitActiveCharacter(FirstCharacter);
					}
					else
					{
						ITaggable::Execute_OnTagOut(NewCharacter);
					}
				}
			}
		}
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

void AZGPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Move 바인딩
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZGPPlayerController::HandleMove);
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
			EnhancedInputComponent->BindAction(IA_LockOn, ETriggerEvent::Started, this, &AZGPPlayerController::HondleLockOn);
		}
	}
}

AActor* AZGPPlayerController::GetCurrentTargetActor_Implementation() const
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

void AZGPPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Move(Value);
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
	if (m_pSquadManagerComponent)
	{
		m_pSquadManagerComponent->RequestTag();
	}
}

void AZGPPlayerController::HondleLockOn()
{
	if (m_pTargetingComponent)
	{
		m_pTargetingComponent->ToggleLockOn();
	}
}



