// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Targetable.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_pSpringArmComponent.IsValid())
	{
		m_bOriginalCameraLag = m_pSpringArmComponent->bEnableCameraLag;
		m_bOriginalCollision = m_pSpringArmComponent->bDoCollisionTest;
	}
}

void UPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsHardLock())
	{
		UpdateHardLockCamera(DeltaTime);
	}
}

void UPlayerCameraComponent::SetHardLockTarget(AActor* Target)
{
	m_pHardLockTarget = Target;
}

void UPlayerCameraComponent::ClearHardLockTarget()
{
	m_pHardLockTarget.Reset();
}

void UPlayerCameraComponent::StopCameraLagTemporary()
{
	if (!m_pSpringArmComponent.IsValid()) return;
	if (m_bCameraLagDisabled) return;

	m_bOriginalCameraLag = m_pSpringArmComponent->bEnableCameraLag;
	m_bOriginalCollision = m_pSpringArmComponent->bDoCollisionTest;

	m_pSpringArmComponent->bEnableCameraLag = false;
	m_pSpringArmComponent->bDoCollisionTest = false;

	m_bCameraLagDisabled = true;
}

void UPlayerCameraComponent::RestoreCameraLag()
{
	if (!m_pSpringArmComponent.IsValid()) return;
	if (!m_bCameraLagDisabled) return;

	m_pSpringArmComponent->bEnableCameraLag = m_bOriginalCameraLag;
	m_pSpringArmComponent->bDoCollisionTest = m_bOriginalCollision;

	m_bCameraLagDisabled = false;
}

void UPlayerCameraComponent::SetSpringArm(USpringArmComponent* SpringArm)
{
	m_pSpringArmComponent = SpringArm;

	if (m_pSpringArmComponent.IsValid())
	{
		m_bOriginalCameraLag = m_pSpringArmComponent->bEnableCameraLag;
		m_bOriginalCollision = m_pSpringArmComponent->bDoCollisionTest;
	}
}

void UPlayerCameraComponent::UpdateHardLockCamera(float DeltaTime)
{
	AActor* Target = m_pHardLockTarget.Get();
	if (!Target) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(Owner);
	if (!OwnerCharacter) return;

	AController* Controller = OwnerCharacter->GetController();
	if (!Controller) return;

	FVector TargetLocation = GetTargetLocation(Target);
	FVector MyLocation = Owner->GetActorLocation();
	FVector Direction = TargetLocation - MyLocation;

	// 수평 방향
	FVector DirectionXY = FVector(Direction.X, Direction.Y, 0.f);
	if (DirectionXY.IsNearlyZero()) return;

	float Distance2D = DirectionXY.Size();
	FRotator TargetYawRotation = DirectionXY.Rotation();

	// Pitch 계산 (타겟 높이 차이)
	float HeightDiff = TargetLocation.Z - MyLocation.Z;

	// 최소 거리 설정 (이 거리 이하에서는 Pitch 변화 억제)
	float EffectiveDistance = FMath::Max(Distance2D, m_fMinDistanceForPitch);

	// Pitch 계산 (완만하게)
	float RawPitch = FMath::RadiansToDegrees(FMath::Atan2(HeightDiff, EffectiveDistance));

	// 거리에 따른 Pitch 블렌딩 (가까울수록 기본 Pitch로)
	float DistanceAlpha = FMath::Clamp(Distance2D / m_fMinDistanceForPitch, 0.f, 1.f);
	float PitchAngle = FMath::Lerp(m_fDefaultPitch, RawPitch, DistanceAlpha);

	// Clamp
	PitchAngle = FMath::Clamp(PitchAngle, m_fPitchClampMin, m_fPitchClampMax);

	FRotator TargetCameraRotation = FRotator(PitchAngle, TargetYawRotation.Yaw, 0.f);

	// 보간
	FRotator CurrentCamRot = Controller->GetControlRotation();
	FRotator NewCamRot = FMath::RInterpTo(CurrentCamRot, TargetCameraRotation, DeltaTime, m_fHardLockRotationSpeed);

	Controller->SetControlRotation(NewCamRot);
}

FVector UPlayerCameraComponent::GetTargetLocation(AActor* Target) const
{
	if (!Target) return FVector::ZeroVector;

	if (Target->Implements<UTargetable>())
	{
		return ITargetable::Execute_GetTargetLocation(Target);
	}

	return Target->GetActorLocation();
}
