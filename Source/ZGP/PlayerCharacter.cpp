// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

#include "ZGPPlayerController.h"
#include "InputAction.h"

#include "ComboComponent.h"
#include "SkillComponent.h"
#include "DodgeComponent.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, -0.f));

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_SpringArm->SetupAttachment(RootComponent);
	m_SpringArm->TargetArmLength = 400.f;
	m_SpringArm->bUsePawnControlRotation = true;

	m_SpringArm->SocketOffset = FVector(0.f, 50.f, 70.f);
	m_SpringArm->bEnableCameraLag = true;
	m_SpringArm->CameraLagSpeed = 10.f;

	m_SpringArm->bDoCollisionTest = true;

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
	m_pComboComp = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComponent"));
	m_pDodgeComp = CreateDefaultSubobject<UDodgeComponent>(TEXT("DodgeComponent"));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementSmoothing(DeltaTime);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 두 컴포넌트의 델리게이트 바인딩
	if (m_pComboComp && m_pSkillComponent)
	{
		m_pComboComp->OnPerformComboAttack.AddDynamic(m_pSkillComponent, &USkillComponent::ExecuteComboAttack);
	}
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

void APlayerCharacter::PerformTagIn(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 기존 타이머 정리
	World->GetTimerManager().ClearTimer(m_CameraLagTimerHandle);

	// 1. 카메라 랙 비활성화
	if (m_SpringArm)
	{
		// 복원 대기 중이 아닐 때만 원본 값 저장
		if (!m_bCameraResetPending)
		{
			m_bCameraLag = m_SpringArm->bEnableCameraLag;
			m_bCameraCollision = m_SpringArm->bDoCollisionTest;
		}
		m_bCameraResetPending = true;

		m_SpringArm->bEnableCameraLag = false;
		m_SpringArm->bDoCollisionTest = false;
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
	if (m_SpringArm)
	{
		//World->GetTimerManager().SetTimer(m_CameraLagTimerHandle, this, &APlayerCharacter::ResetCameraSetting, 0.01f, false);
		World->GetTimerManager().SetTimerForNextTick(this, &APlayerCharacter::ResetCameraSetting);
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

	UE_LOG(LogTemp, Log, TEXT("[PlayerCharacter] : %s : Deferred Tag Out Complete"), *GetName());
}

void APlayerCharacter::SetActionTagOutState(bool bActive)
{
	if (bActive)
	{
		if (m_pDodgeComp)
		{
			m_pDodgeComp->SetInvincible(true);
		}

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
	}
	else
	{
		if (m_pDodgeComp)
		{
			m_pDodgeComp->SetInvincible(false);
		}

		UCapsuleComponent* Capsule = GetCapsuleComponent();
		if (Capsule)
		{
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		}
	}
}

void APlayerCharacter::ResetCameraSetting()
{
	if (m_SpringArm)
	{
		m_SpringArm->bEnableCameraLag = m_bCameraLag;
		m_SpringArm->bDoCollisionTest = m_bCameraCollision;
	}
	m_bCameraResetPending = false;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	m_vRawInput = Value.Get<FVector2D>();
}

void APlayerCharacter::StopMove()
{
	m_vRawInput = FVector2D::ZeroVector;
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::RequestAttack()
{
	if (m_pComboComp)
	{
		m_pComboComp->RequestComboAttack();
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

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActionState(EActionState::IDLE);
}

void APlayerCharacter::OnTagOutAction_Implementation()
{
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

void APlayerCharacter::ApplyCombatEffect_Implementation(const FDamageData& DamageData)
{
	if (m_pDodgeComp && m_pDodgeComp->IsInvincible())
	{
		bool bSuccess = m_pDodgeComp->TryPerfectDodgeTrigger();
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

void APlayerCharacter::RequestDodge()
{
	if (!m_pDodgeComp) return;

	FVector DodgeDirection = FVector::ZeroVector;
	DodgeDirection = GetLastMovementInputVector();

	m_pDodgeComp->RequestDodge(DodgeDirection);
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


void APlayerCharacter::UpdateMovementSmoothing(float DeltaTime)
{
	UpdateInputSmoothing(DeltaTime);

	FVector MoveDirection = FVector::ZeroVector;

	if (!m_vSmoothedInput.IsNearlyZero())
	{
		if (Controller != nullptr)
		{
			const FRotator ControlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			MoveDirection = (ForwardDirection * m_vSmoothedInput.Y) + (RightDirection * m_vSmoothedInput.X);
			MoveDirection.Z = 0.f;

			if (!MoveDirection.IsNearlyZero())
			{
				MoveDirection.Normalize();
			}
		}
	}

	UpdateDirectionChange(DeltaTime, MoveDirection);

	// 실제 입력이 있을 때만 회전 보간 적용
	if (!m_vRawInput.IsNearlyZero() && !MoveDirection.IsNearlyZero())
	{
		const FQuat CurrentQuat = GetActorQuat();

		const FRotator TargetRotator = MoveDirection.Rotation();
		const FQuat TargetQuat = FQuat(FRotator(0.f, TargetRotator.Yaw, 0.f));

		const FQuat NewQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, m_fInterpRotationSpeed);

		SetActorRotation(NewQuat);
	}

	ApplyMovementSmoothing(MoveDirection);
}

void APlayerCharacter::UpdateInputSmoothing(float DeltaTime)
{
	m_vSmoothedInput.X = FMath::FInterpTo(m_vSmoothedInput.X, m_vRawInput.X, DeltaTime, m_fInterpInputSpeed);
	m_vSmoothedInput.Y = FMath::FInterpTo(m_vSmoothedInput.Y, m_vRawInput.Y, DeltaTime, m_fInterpInputSpeed);
}

void APlayerCharacter::UpdateDirectionChange(float DeltaTime, const FVector& CurrentDirection)
{
	if (CurrentDirection.IsNearlyZero())
	{
		// 입력이 없을 시 속도 배율 회복
		m_fCurrentSpeedMultiply = FMath::FInterpTo(m_fCurrentSpeedMultiply, 1.0f, DeltaTime, m_fBackSpeed);
		return;
	}

	if (m_vLastDirection.IsNearlyZero())
	{
		// 첫 이동
		m_vLastDirection = CurrentDirection;
		m_fCurrentSpeedMultiply = 1.0f;
		return;
	}

	// 이전 방향과 현재 방향 각도 계산
	const float DirectionDot = FVector::DotProduct(m_vLastDirection, CurrentDirection);

	// Dot = 1 , Dot = -1(반대)
	float NormalizeDot = (DirectionDot + 1.0f) * 0.5f;		// 정규화 (0 ~ 1)

	// 최저 속도와 최고 속도 보간
	float Target = m_fMinSpeedDirectionChange + ((1.0f - m_fMinSpeedDirectionChange) * NormalizeDot);
	Target = FMath::Clamp(Target, m_fMinSpeedDirectionChange, 1.0f);

	// 급격한 방향 전환
	if (Target < m_fMinSpeedDirectionChange)
	{
		// 즉시 감속
		m_fCurrentSpeedMultiply = Target;
	}
	else
	{
		// 회복은 서서히
		m_fCurrentSpeedMultiply = FMath::FInterpTo(m_fCurrentSpeedMultiply, Target, DeltaTime, m_fBackSpeed);
	}

	m_vLastDirection = CurrentDirection;
}

void APlayerCharacter::UpdateRotationSmoothing(float DeltaTime, const FVector& MoveDirection)
{
	if (MoveDirection.IsNearlyZero()) return;

	const FRotator TargetRotation = MoveDirection.Rotation();
	const FRotator CurrentRotation = GetActorRotation();

	const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, m_fInterpRotationSpeed);

	SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
}

void APlayerCharacter::ApplyMovementSmoothing(const FVector& MoveDirection)
{
	if (MoveDirection.IsNearlyZero()) return;

	// 아날로그 스틱 지원
	const float Input = FMath::Clamp(m_vSmoothedInput.Size(), 0.f, 1.f);

	const float Scale = Input * m_fCurrentSpeedMultiply;

	AddMovementInput(MoveDirection, Scale);
}