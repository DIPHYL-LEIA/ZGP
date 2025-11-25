// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "Targetable.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTargetingComponent::UpdateTargeting(float DeltaTime, const FVector& Location, const FVector& Forward, AActor* IgnoreActor)
{
	//	1. 기본 검사
	if (m_pCurrentTarget.IsValid())
	{
		float DistanceSquared = FVector::DistSquared(Location, m_pCurrentTarget->GetActorLocation());
		float MaxDistanceSquared = m_fSearchRadius * m_fSearchRadius * m_fHardLockRelease;

		// 거리 멀어졌을 때
		if (DistanceSquared > MaxDistanceSquared)
		{
			ChangeTarget(nullptr);

			if (m_eTargetingMode == ETargetingMode::HARD_LOCK)
			{
				m_eTargetingMode = ETargetingMode::SOFT_LOCK;
				m_fLostTargetTimer = 0.f;
			}
		}
	}
	else
	{
		// Hard Lock 대상 죽음
		if (m_eTargetingMode == ETargetingMode::HARD_LOCK)
		{
			m_eTargetingMode = ETargetingMode::SOFT_LOCK;
			m_fLostTargetTimer = 0.f;
		}
	}

	//	2. 모드별 처리
	if (m_eTargetingMode == ETargetingMode::HARD_LOCK)
	{
		m_fHardLockTimer += DeltaTime;
		if (m_fHardLockTimer >= m_fHardLockInterval)
		{
			m_fHardLockTimer = 0.f;
			SetHardLock(DeltaTime, Location);
		}
	}
	else
	{
		m_fSoftLockTimer += DeltaTime;
		if (m_fSoftLockTimer >= m_fSoftLockInterval)
		{
			m_fSoftLockTimer = 0.f;
			SetSoftLock(Location, Forward, IgnoreActor);
		}
	}
}

void UTargetingComponent::ToggleLockOn()
{
	if (m_eTargetingMode == ETargetingMode::SOFT_LOCK)
	{
		if (!m_pCurrentTarget.IsValid()) return;

		if (m_pCurrentTarget->Implements<UTargetable>())
		{
			if (!ITargetable::Execute_IsTargetable(m_pCurrentTarget.Get())) return;
		}

		m_eTargetingMode = ETargetingMode::HARD_LOCK;
		m_fLostTargetTimer = 0.f;

		if (m_pCurrentTarget->Implements<UTargetable>())
		{
			ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), true);
		}
	}
	else
	{
		ReleaseLock();
	}
}

AActor* UTargetingComponent::GetCurrentTarget() const
{
	return m_pCurrentTarget.Get();
}

ETargetingMode UTargetingComponent::GetTargetingMode() const
{
	return m_eTargetingMode;
}

bool UTargetingComponent::IsHardLock() const
{
	return (m_eTargetingMode == ETargetingMode::HARD_LOCK);
}

void UTargetingComponent::SetSoftLock(const FVector& Location, const FVector& Forward, AActor* IgnoreActor)
{
	//	1. 탐색 범위 세팅
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	if (IgnoreActor)
	{
		QueryParams.AddIgnoredActor(IgnoreActor);
	}

	World->OverlapMultiByChannel(
		OverlapResults, Location, FQuat::Identity, ECC_Pawn/*적 채널*/,
		FCollisionShape::MakeSphere(m_fSearchRadius), QueryParams);

	if (m_bShowDebug)
	{
		DrawDebugSphere(World, Location, m_fSearchRadius, 24, FColor::Emerald, false, m_fSoftLockInterval);
	}

	//	2. 최적 타겟 검색
	AActor* BestTarget = nullptr;
	float BestScore = -1.f;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Actor = Result.GetActor();
		if (!Actor) continue;
		if (!Actor->Implements<UTargetable>()) continue;
		if (!ITargetable::Execute_IsTargetable(Actor)) continue;

		// 시야각
		if (!IsInViewAngle(Actor, Location, Forward)) continue;
		// 가시성
		if (!IsTargetVisible(Actor, Location)) continue;
		// 점수
		float Score = CalculateScore(Actor, Location, Forward);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Actor;
		}
	}

	//	3. 타겟 변경
	if (m_pCurrentTarget.Get() != BestTarget)
	{
		ChangeTarget(BestTarget);
	}
}

void UTargetingComponent::SetHardLock(float DeltaTime, const FVector& Location)
{
	if (!m_pCurrentTarget.IsValid())
	{
		ReleaseLock();
		return;
	}

	//	1. 타겟 상태 검사
	if (m_pCurrentTarget->Implements<UTargetable>())
	{
		if (!ITargetable::Execute_IsTargetable(m_pCurrentTarget.Get()))
		{
			ReleaseLock();
			return;
		}
	}

	//	2. 가시성
	if (!IsTargetVisible(m_pCurrentTarget.Get(), Location))
	{
		m_fLostTargetTimer += DeltaTime;

		if (m_fLostTargetTimer >= m_fHardLockLostTarget)
		{
			ReleaseLock();
		}
	}
	else
	{
		m_fLostTargetTimer = 0.f;
	}
}

float UTargetingComponent::CalculateScore(AActor* TargetActor, const FVector& Location, const FVector& Forward)
{
	if (!TargetActor)
		return -1.f;

	FVector TargetLocation = TargetActor->GetActorLocation();
	if (TargetActor->Implements<UTargetable>())
	{
		// 타겟 가능 상태인가
		if (!ITargetable::Execute_IsTargetable(TargetActor))
		{
			return -1.f;
		}
		TargetLocation = ITargetable::Execute_GetTargetLocation(TargetActor);
	}

	// 각도(2D, Z축 제외)
	FVector LocationXY = FVector(Location.X, Location.Y, 0.f);
	FVector TargetLocationXY = FVector(TargetLocation.X, TargetLocation.Y, 0.f);
	FVector ForwardXY = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();

	FVector TargetDirectionXY = (TargetLocationXY - LocationXY).GetSafeNormal();
	float Dot = FVector::DotProduct(ForwardXY, TargetDirectionXY);

	// 시야각 밖
	if (Dot < m_fSearchAngle)
		return -1.f;

	// 각도 정규화 (0~1)
	float AngleScore = (Dot - m_fSearchAngle) / (1.0f - m_fSearchAngle);

	// 거리(3D)
	float DistanceSquared = FVector::DistSquared(Location, TargetLocation);
	float MaxDistanceSquared = m_fSearchRadius * m_fSearchRadius;
	float DistanceScore = 1.0f - FMath::Clamp(DistanceSquared / MaxDistanceSquared, 0.f, 1.f);

	// 최종 점수 (각도 60, 거리 40)
	float FinalScore = (AngleScore * m_fAngleWeight) + (DistanceScore * m_fDistanceWeight);

	// 히스테리시스
	if (TargetActor == m_pCurrentTarget.Get())
	{
		FinalScore += m_fHysteresisValue;
	}

	return FinalScore;
}

bool UTargetingComponent::IsTargetVisible(AActor* TargetActor, const FVector& StartLocation) const
{
	return false;
}

bool UTargetingComponent::IsInViewAngle(AActor* TargetActor, const FVector& Location, const FVector& Forward) const
{
	return false;
}

void UTargetingComponent::ChangeTarget(AActor* NewTarget)
{
	// 기존 타겟 제거
	if (m_pCurrentTarget.IsValid() && m_pCurrentTarget->Implements<UTargetable>())
	{
		ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), false);
		ITargetable::Execute_OnUnTargeted(m_pCurrentTarget.Get());
	}

	m_pCurrentTarget = NewTarget;

}

void UTargetingComponent::ReleaseLock()
{
	if (m_eTargetingMode == ETargetingMode::HARD_LOCK)
	{
		if (m_pCurrentTarget.IsValid() && m_pCurrentTarget->Implements<UTargetable>())
		{
			ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), false);
		}
	}

	m_eTargetingMode = ETargetingMode::SOFT_LOCK;
	m_fLostTargetTimer = 0.f;
}
