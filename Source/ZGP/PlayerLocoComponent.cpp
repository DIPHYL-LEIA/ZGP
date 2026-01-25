// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLocoComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Targetable.h"

UPlayerLocoComponent::UPlayerLocoComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerLocoComponent::BeginPlay()
{
	Super::BeginPlay();
	
	m_pOwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UPlayerLocoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!m_pOwnerCharacter.IsValid()) return;

	// 공격 흡착 우선 처리
	if (m_bIsAttackHoming)
	{
		UpdateAttackHoming(DeltaTime);
	}

	// 입력 보간
	UpdateInputSmoothing(DeltaTime);

	// Hard Lock
	if (IsHardLock())
	{
		UpdateHardLockMovement(DeltaTime);
	}
	else
	{
		UpdateNormalMovement(DeltaTime);
	}
}

void UPlayerLocoComponent::SetRawInput(const FVector2D& Input)
{
	m_vRawInput = Input;
}

void UPlayerLocoComponent::ClearInput()
{
	m_vRawInput = FVector2D::ZeroVector;
}

void UPlayerLocoComponent::SetHardLockTarget(AActor* Target)
{
	m_pHardLockTarget = Target;
}

void UPlayerLocoComponent::ClearHardLockTarget()
{
	m_pHardLockTarget.Reset();
}

void UPlayerLocoComponent::StartAttackHoming(AActor* Target)
{
	if (!Target || !m_pOwnerCharacter.IsValid())
	{
		m_bIsAttackHoming = false;
		return;
	}

	FVector MyLocation = m_pOwnerCharacter->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	float Distance = FVector::Dist2D(MyLocation, TargetLocation);

	// 거리 체크
	if (Distance > m_fHomingDistance || Distance <= m_fHomingStopDistance)
	{
		RotateCharacterToTarget(Target, 0.f, true);
		m_bIsAttackHoming = false;
		return;
	}

	RotateCharacterToTarget(Target, 0.f, true);
	m_bIsAttackHoming = true;
	m_pHomingTarget = Target;
	m_vHomingTargetLocation = TargetLocation;
}

void UPlayerLocoComponent::StopAttackHoming()
{
	m_bIsAttackHoming = false;
	m_pHomingTarget.Reset();
}

void UPlayerLocoComponent::UpdateNormalMovement(float DeltaTime)
{
	FVector MoveDirection = CalculateMoveDirection();

	UpdateDirectionChange(DeltaTime, MoveDirection);

	// 회전(입력 있을 때)
	if (!m_vRawInput.IsNearlyZero() && !MoveDirection.IsNearlyZero())
	{
		ACharacter* Owner = m_pOwnerCharacter.Get();
		if (Owner)
		{
			// 짐벌락 방지
			const FQuat CurrentQuat = Owner->GetActorQuat();
			const FRotator TargetRotator = MoveDirection.Rotation();
			const FQuat TargetQuat = FQuat(FRotator(0.f, TargetRotator.Yaw, 0.f));
			const FQuat NewQuat = FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, m_fInterpRotationSpeed);

			Owner->SetActorRotation(NewQuat);
		}
	}
	ApplyMovement(MoveDirection, m_fCurrentSpeedMultiply);
}

void UPlayerLocoComponent::UpdateHardLockMovement(float DeltaTime)
{
	ACharacter* Owner = m_pOwnerCharacter.Get();
	AActor* Target = m_pHardLockTarget.Get();

	if (!Owner || !Target) return;

	// 타겟 방향으로 캐릭터 회전
	RotateCharacterToTarget(Target, DeltaTime, false);

	// Strafe
	if (m_vSmoothInput.IsNearlyZero()) return;

	const FVector Forward = Owner->GetActorForwardVector();
	const FVector Right = Owner->GetActorRightVector();

	FVector MoveDirection = (Forward * m_vSmoothInput.Y) + (Right * m_vSmoothInput.X);
	MoveDirection.Z = 0.f;

	if (!MoveDirection.IsNearlyZero())
	{
		MoveDirection.Normalize();
	}

	// Hard Lock에서는 방향 전환 감속 없음
	const float InputMagnitude = FMath::Clamp(m_vSmoothInput.Size(), 0.f, 1.f);
	ApplyMovement(MoveDirection, InputMagnitude);
}

void UPlayerLocoComponent::UpdateAttackHoming(float DeltaTime)
{
	ACharacter* Owner = m_pOwnerCharacter.Get();
	if (!Owner)
	{
		m_bIsAttackHoming = false;
		return;
	}

	// 타겟 위치 갱신
	if (m_pHomingTarget.IsValid())
	{
		m_vHomingTargetLocation = GetTargetLocation(m_pHomingTarget.Get());
	}

	FVector MyLocation = Owner->GetActorLocation();
	FVector Direction = m_vHomingTargetLocation - MyLocation;
	Direction.Z = 0.f;

	float Distance = Direction.Size();

	// 목표 거리 도달 시 중단
	if (Distance <= m_fHomingStopDistance)
	{
		m_bIsAttackHoming = false;
		return;
	}

	// 이동
	Direction.Normalize();
	FVector Move = Direction * m_fHomingSpeed * DeltaTime;

	// 오버슈팅 방지
	float RemainDistance = Distance - m_fHomingStopDistance;
	if (Move.Size() > RemainDistance)
	{
		Move = Direction * RemainDistance;
		m_bIsAttackHoming = false;
	}
	Owner->AddActorWorldOffset(Move, true);
}

void UPlayerLocoComponent::UpdateInputSmoothing(float DeltaTime)
{
	m_vSmoothInput.X = FMath::FInterpTo(m_vSmoothInput.X, m_vRawInput.X, DeltaTime, m_fInterpInputSpeed);
	m_vSmoothInput.Y = FMath::FInterpTo(m_vSmoothInput.Y, m_vRawInput.Y, DeltaTime, m_fInterpInputSpeed);
}

void UPlayerLocoComponent::UpdateDirectionChange(float DeltaTime, const FVector& CurrentDirection)
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

void UPlayerLocoComponent::ApplyMovement(const FVector& MoveDirection, float SpeedMultiplier)
{
	if (MoveDirection.IsNearlyZero()) return;

	ACharacter* Owner = m_pOwnerCharacter.Get();
	if (!Owner) return;

	const float InputMagnitude = FMath::Clamp(m_vSmoothInput.Size(), 0.f, 1.f);
	const float FinalScale = InputMagnitude * SpeedMultiplier;

	Owner->AddMovementInput(MoveDirection, FinalScale);
}

FVector UPlayerLocoComponent::CalculateMoveDirection() const
{
	if (m_vSmoothInput.IsNearlyZero()) return FVector::ZeroVector;

	ACharacter* Owner = m_pOwnerCharacter.Get();
	if (!Owner) return FVector::ZeroVector;

	AController* Controller = Owner->GetController();
	if (!Controller) return FVector::ZeroVector;

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector MoveDirection = (ForwardDirection * m_vSmoothInput.Y) + (RightDirection * m_vSmoothInput.X);
	MoveDirection.Z = 0.f;

	if (!MoveDirection.IsNearlyZero())
	{
		MoveDirection.Normalize();
	}

	return MoveDirection;
}

FVector UPlayerLocoComponent::GetTargetLocation(AActor* Target) const
{
	if (!Target) return FVector::ZeroVector;

	if (Target->Implements<UTargetable>())
	{
		return ITargetable::Execute_GetTargetLocation(Target);
	}

	return Target->GetActorLocation();
}

void UPlayerLocoComponent::RotateCharacterToTarget(AActor* Target, float DeltaTime, bool bInstant)
{
	ACharacter* Owner = m_pOwnerCharacter.Get();
	if (!Owner || !Target) return;

	FVector TargetLocation = GetTargetLocation(Target);
	FVector MyLocation = Owner->GetActorLocation();
	FVector Direction = TargetLocation - MyLocation;
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero()) return;

	FRotator TargetRotation = Direction.Rotation();

	if (bInstant)
	{
		Owner->SetActorRotation(FRotator(0.f, TargetRotation.Yaw, 0.f));
	}
	else
	{
		FRotator CurrentRotation = Owner->GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, m_fInterpRotationSpeed);
		Owner->SetActorRotation(FRotator(0.f, NewRotation.Yaw, 0.f));
	}
}

