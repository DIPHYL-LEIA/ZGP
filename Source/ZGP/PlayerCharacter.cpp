// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "InputActionValue.h"

#include "ComboComponent.h"
#include "SkillComponent.h"
#include "DodgeComponent.h"
#include "PlayerLocoComponent.h"
#include "PlayerCameraComponent.h"

#include "TargetProvider.h"
#include "Targetable.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, -0.f));

	// Spring Arm
	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_SpringArm->SetupAttachment(RootComponent);
	m_SpringArm->TargetArmLength = 400.f;
	m_SpringArm->bUsePawnControlRotation = true;

	m_SpringArm->SocketOffset = FVector(0.f, 50.f, 70.f);
	m_SpringArm->bEnableCameraLag = true;
	m_SpringArm->CameraLagSpeed = 10.f;
	m_SpringArm->ProbeChannel = ECC_Visibility;

	m_SpringArm->bDoCollisionTest = true;

	// Camera
	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Camera->SetupAttachment(m_SpringArm);
	m_Camera->bUsePawnControlRotation = false;

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->GroundFriction = 8.0f;

	// Component
	m_pSkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	m_pComboComponent = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComponent"));
	m_pDodgeCompComponent = CreateDefaultSubobject<UDodgeComponent>(TEXT("DodgeComponent"));
	m_pPlayerLocoComponent = CreateDefaultSubobject<UPlayerLocoComponent>(TEXT("PlayerLocoComponent"));
	m_pPlayerCameraComponent = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("PlayerCameraComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 두 컴포넌트의 델리게이트 바인딩
	if (m_pSkillComponent)
	{
		m_pSkillComponent->OnRequestPlayMontage.AddDynamic(this, &ABaseCharacter::HandlePlayMontage);
	}
	if (m_pComboComponent && m_pSkillComponent)
	{
		m_pComboComponent->OnPerformComboAttack.AddDynamic(m_pSkillComponent, &USkillComponent::ExecuteComboAttack);
	}

	if (m_pPlayerCameraComponent && m_SpringArm)
	{
		m_pPlayerCameraComponent->SetSpringArm(m_SpringArm);
	}

	if (m_pDodgeCompComponent)
	{
		m_pDodgeCompComponent->OnPerfectDodge.AddDynamic(this, &APlayerCharacter::HandlePerfectDodge);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->SetRawInput(Value.Get<FVector2D>());
	}
}

void APlayerCharacter::StopMove()
{
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->ClearInput();
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// Hard Lock 시 마우스 회전 무시
	if (IsHardLock()) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::RequestAttack()
{
	OnPlayerAttackStart.Broadcast();

	// 공격 전 타겟 방향 흡착 준비
	AActor* Target = GetCurrentTarget();
	if (m_pPlayerLocoComponent && Target)
	{
		m_pPlayerLocoComponent->StartAttackHoming(Target);
	}

	if (m_pComboComponent)
	{
		m_pComboComponent->RequestComboAttack();
	}
}

void APlayerCharacter::RequestDodge()
{
	if (!m_pDodgeCompComponent) return;

	FVector DodgeDirection = FVector::ZeroVector;
	DodgeDirection = GetLastMovementInputVector();

	m_pDodgeCompComponent->RequestDodge(DodgeDirection);
}

void APlayerCharacter::SetHardLockTarget(AActor* Target)
{
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->SetHardLockTarget(Target);
	}

	if (m_pPlayerCameraComponent)
	{
		m_pPlayerCameraComponent->SetHardLockTarget(Target);
	}
}

void APlayerCharacter::ClearHardLockTarget()
{
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->ClearHardLockTarget();
	}

	if (m_pPlayerCameraComponent)
	{
		m_pPlayerCameraComponent->ClearHardLockTarget();
	}
}

AActor* APlayerCharacter::GetCurrentTarget() const
{
	AController* PC = GetController();
	if (PC && PC->Implements<UTargetProvider>())
	{
		return ITargetProvider::Execute_GetCurrentTargetActor(PC);
	}
	return nullptr;
}

bool APlayerCharacter::IsHardLock() const
{
	AController* PC = GetController();
	if (PC && PC->Implements<UTargetProvider>())
	{
		return ITargetProvider::Execute_IsTargetLock(PC);
	}
	return false;
}


void APlayerCharacter::PerformTagIn(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (m_pPlayerCameraComponent)
	{
		m_pPlayerCameraComponent->StopCameraLagTemporary();
	}

	// 2. 핵심 로직 (반드시 실행)
	SetActorLocation(TargetLocation);
	SetActorRotation(TargetRotation);

	// 3. SpringArm Transform 강제 갱신 (B->A 튐 방지)
	if (m_SpringArm)
	{
		m_SpringArm->UpdateChildTransforms();
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActionTagOutState(false);
	SetActionState(EActionState::IDLE);

	// 4. 카메라 복원 예약
	if (m_pPlayerCameraComponent)
	{
		World->GetTimerManager().SetTimerForNextTick([this]()
			{
				if (m_pPlayerCameraComponent)
				{
					m_pPlayerCameraComponent->RestoreCameraLag();
				}
			});
	}
}

void APlayerCharacter::ExecuteActionTagOut()
{
	// 타이머 정리
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(ForceTagOutTimerHandle);
	}

	// 델리게이트 정리
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::HandleActionMontageEnded);
	}

	m_bPendingTagOut = false;

	// 상태 복원 후 숨김
	SetActionTagOutState(false);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActionState(EActionState::IDLE);
}

void APlayerCharacter::SetActionTagOutState(bool bActive)
{
	if (bActive)
	{
		if (m_pDodgeCompComponent)
		{
			m_pDodgeCompComponent->SetInvincible(true);
		}

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
	}
	else
	{
		if (m_pDodgeCompComponent)
		{
			m_pDodgeCompComponent->SetInvincible(false);
		}

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		}
	}
}

void APlayerCharacter::HandleSkillMontageEnded()
{
	// 공격 흡착 해제
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->StopAttackHoming();
	}
	if (m_bIsChainAttack)
	{
		m_bIsChainAttack = false;
		OnChainAttackSkillFinish.Broadcast();
	}
}

void APlayerCharacter::HandlePerfectDodge()
{
	OnPlayerPerfectDodge.Broadcast();
}

void APlayerCharacter::HandleActionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &APlayerCharacter::HandleActionMontageEnded);
	}

	if (m_bPendingTagOut)
	{
		ExecuteActionTagOut();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool APlayerCharacter::CanTag_Implementation() const
{
	if (IsActionState(EActionState::HIT) ||
		IsActionState(EActionState::DODGING) || IsActionState(EActionState::DEAD))
	{
		return false;
	}
	return true;
}

void APlayerCharacter::OnTagIn_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	PerformTagIn(TargetLocation, TargetRotation);

	SetActionState(EActionState::IDLE);
}

void APlayerCharacter::OnTagOut_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s : Execute Tag Out "), *GetName());

	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->ClearInput();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActionState(EActionState::IDLE);
}

void APlayerCharacter::OnTagOutAction_Implementation()
{
	if (m_pPlayerLocoComponent)
	{
		m_pPlayerLocoComponent->ClearInput();
	}
	// 태그아웃 대기 상태 (무적, 충돌 무시)
	SetActionTagOutState(true);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		m_bPendingTagOut = true;
		AnimInstance->OnMontageEnded.AddDynamic(this, &APlayerCharacter::HandleActionMontageEnded);
	}
	else
	{
		ExecuteActionTagOut();
		return;
	}

	// 일정 시간 후 강제 태그 아웃
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(ForceTagOutTimerHandle, this, &APlayerCharacter::ExecuteActionTagOut, m_fForceTagOutDelay, false);
	}
}

void APlayerCharacter::OnChainAttackTag_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation, AActor* TargetEnemy)
{
	PerformTagIn(TargetLocation, TargetRotation);

	SetActionState(EActionState::ATTACKING);

	m_bIsChainAttack = true;

	if (m_pSkillComponent)
	{
		m_pSkillComponent->ExecuteSkillID(m_ChainAttackSkillID);
	}
}

bool APlayerCharacter::IsTargetable_Implementation() const
{
	if (IsActionState(EActionState::DEAD)) return false;
	if (IsHidden()) return false;
	if (!GetActorEnableCollision()) return false;

	return true;
}

FVector APlayerCharacter::GetTargetLocation_Implementation() const
{
	if (GetMesh())
	{
		return GetMesh()->GetSocketLocation(TEXT("spine_03"));
	}
	return GetActorLocation();
}

void APlayerCharacter::OnTargeted_Implementation(bool IsTargeted)
{
}

void APlayerCharacter::OnUnTargeted_Implementation()
{
}

bool APlayerCharacter::ExecuteSkillByID_Implementation(FName SkillID)
{
	if (m_pSkillComponent)
	{
		return m_pSkillComponent->ExecuteSkillID(SkillID);
	}
	return false;
}

bool APlayerCharacter::IsExecuteSkill_Implementation() const
{
	if (m_pSkillComponent)
	{
		return !m_pSkillComponent->GetCurrentSkillID().IsNone();
	}

	return false;
}

bool APlayerCharacter::IsCurrentSkillHeavy_Implementation() const
{
	if (m_pSkillComponent)
	{
		return m_pSkillComponent->IsCurrentSkillHeavy();
	}
	return false;
}

void APlayerCharacter::NotifySkillCompleted_Implementation()
{
	if (m_pSkillComponent)
	{
		m_pSkillComponent->NotifySkillCompleted();
	}
	HandleSkillMontageEnded();
}

FName APlayerCharacter::GetCurrentSkillID_Implementation() const
{
	if (m_pSkillComponent)
	{
		return m_pSkillComponent->GetCurrentSkillID();
	}
	return NAME_None;
}

void APlayerCharacter::ApplyCombatEffect_Implementation(const FDamageData& DamageData)
{
	if (m_pDodgeCompComponent && m_pDodgeCompComponent->IsInvincible())
	{
		bool bSuccess = m_pDodgeCompComponent->TryPerfectDodgeTrigger();
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Player] Perfect Dodge executed!"));
		}
		return;
	}

	// HitReaction이 Light일 때 Hit 상태 진입하지 않음
	if (DamageData.HitReaction == EHitReactionType::LIGHT) return;

	Super::ApplyCombatEffect_Implementation(DamageData);
}

void APlayerCharacter::RequestParryAttack(AActor* ParriedEnemy)
{
	if (ParriedEnemy)
	{
		FVector TargetLocation = ParriedEnemy->GetActorLocation();
		FVector MyLocation = GetActorLocation();

		FVector Direction = TargetLocation - MyLocation;
		Direction.Z = 0.f;

		if (!Direction.IsNearlyZero())
		{
			FRotator LookRotation = Direction.Rotation();
			SetActorRotation(LookRotation);

			// 카메라 회전 동기화
			if (Controller)
			{
				Controller->SetControlRotation(LookRotation);
			}
		}
	}

	if (m_pSkillComponent)
	{
		m_pSkillComponent->ExecuteSkillID(m_ParryAttackSkillID);
	}
}
