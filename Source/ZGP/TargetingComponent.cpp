// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "Targetable.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_bIsLockOn = false;
}

void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTargetingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UTargetingComponent::UpdateTargeting(const FVector& Location, const FVector& Forward, AActor* IgnoreActor)
{
	// 락온 체크
	if (m_bIsLockOn)
	{
		if (m_pCurrentTarget.IsValid())
		{
			if (m_pCurrentTarget->Implements<UTargetable>() && !ITargetable::Execute_IsTargetable(m_pCurrentTarget.Get()))
			{
				ToggleLockOn();
			}
			else
			{
				float DistanceSquared = FVector::DistSquared(Location, m_pCurrentTarget->GetActorLocation());
				if (DistanceSquared > (m_fSearchRadius * m_fSearchRadius) * 1.5f)
				{
					ToggleLockOn();
					ChangeTarget(nullptr);
				}
			}
		}
		else
		{
			ToggleLockOn();
		}
		return;		// 락온 중에는 새 타겟 찾지 않음
	}


	// 타겟 탐색
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());		// 컨트롤러 제외
	if (IgnoreActor)
	{
		QueryParams.AddIgnoredActor(IgnoreActor);	// 내 캐릭터 제외
	}

	// 오버랩 성능 최적화 필요
	bool bInteraction = World->OverlapMultiByChannel(
		OverlapResults, Location, FQuat::Identity, ECollisionChannel::ECC_Pawn, FCollisionShape::MakeSphere(m_fSearchRadius), QueryParams
	);

	if (m_bShowDebugShape)
	{
		DrawDebugSphere(World, Location, m_fSearchRadius, 32, FColor::Emerald, false, 0.1f);
	}

	// 점수 계산
	AActor* BestActor = nullptr;
	float BestScore = -1.0f;

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Actor = Result.GetActor();

		if (!Actor || !Actor->Implements<UTargetable>())
			continue;
		if (!ITargetable::Execute_IsTargetable(Actor))
			continue;

		float Score = CalculateScore(Actor, Location, Forward);

		if (Score > BestScore)
		{
			BestScore = Score;
			BestActor = Actor;
		}
	}

	// 타겟 갱신
	if (m_pCurrentTarget.Get() != BestActor)
	{
		ChangeTarget(BestActor);
	}

}

AActor* UTargetingComponent::GetCurrentTarget() const
{
	return m_pCurrentTarget.Get();
}

void UTargetingComponent::SetCurrentTarget(AActor* NewTarget)
{
	if (m_pCurrentTarget.Get() == NewTarget) return;
	ChangeTarget(NewTarget);
}

ETargetingMode UTargetingComponent::GetTargetingMode() const
{
	return m_eTargetingMode;
}

void UTargetingComponent::ToggleLockMode()
{
}

void UTargetingComponent::ToggleLockOn()
{
	if (!m_pCurrentTarget.IsValid() || !m_pCurrentTarget->Implements<UTargetable>())
	{
		m_bIsLockOn = false;
		return;
	}

	m_bIsLockOn = !m_bIsLockOn;

	ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), m_bIsLockOn);
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

void UTargetingComponent::ChangeTarget(AActor* NewTarget)
{
	// 기존 타겟 제거
	if (m_pCurrentTarget.IsValid() && m_pCurrentTarget->Implements<UTargetable>())
	{
		ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), false);
		ITargetable::Execute_OnUnTargeted(m_pCurrentTarget.Get());
	}

	m_pCurrentTarget = NewTarget;

	if (m_pCurrentTarget.IsValid() && m_pCurrentTarget->Implements<UTargetable>())
	{
		ITargetable::Execute_OnTargeted(m_pCurrentTarget.Get(), m_bIsLockOn);
	}
}
