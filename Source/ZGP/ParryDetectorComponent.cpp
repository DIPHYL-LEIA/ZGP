// Fill out your copyright notice in the Description page of Project Settings.


#include "ParryDetectorComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "CombatInteraction.h"
#include "TargetProvider.h"
#include "Engine/OverlapResult.h"

UParryDetectorComponent::UParryDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UParryDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UParryDetectorComponent::CanParry() const
{
	AActor* ActiveCharacter = GetActiveCharacter();
	if (!ActiveCharacter) return false;

	const FVector Location = ActiveCharacter->GetActorLocation();
	const FVector Forward = ActiveCharacter->GetActorForwardVector();

	TArray<AActor*> NearActors;
	FindParryableActors(NearActors);

	for (AActor* Actor : NearActors)
	{
		if (IsValidParryTarget(Actor, Location, Forward))
		{
			return true;
		}
	}

	return false;
}

AActor* UParryDetectorComponent::GetParryTarget() const
{
	AActor* ActiveCharacter = GetActiveCharacter();
	if (!ActiveCharacter) return nullptr;

	const FVector Location = ActiveCharacter->GetActorLocation();
	const FVector Forward = ActiveCharacter->GetActorForwardVector();
	const float MaxDistanceSquared = m_fParryMaxDistance * m_fParryMaxDistance;

	TArray<AActor*> NearActors;
	FindParryableActors(NearActors);

	if (NearActors.Num() == 0) return nullptr;

	// 1 순위 : 현재 타겟이 패리 가능이면 즉시 반환
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UTargetProvider>())
	{
		AActor* CurrentTarget = ITargetProvider::Execute_GetCurrentTargetActor(Owner);
		if (CurrentTarget && NearActors.Contains(CurrentTarget))
		{
			if (IsValidParryTarget(CurrentTarget, Location, Forward))
			{
				return CurrentTarget;
			}
		}
	}

	// 2 순위 : 가까운 타겟
	AActor* CloseTarget = nullptr;
	float CloseDistanceSquared = MaxDistanceSquared;

	for (AActor* Actor : NearActors)
	{
		if (!IsValidParryTarget(Actor, Location, Forward))
		{
			continue;
		}

		float DistanceSquared = FVector::DistSquared(Location, Actor->GetActorLocation());

		if (DistanceSquared < CloseDistanceSquared)
		{
			CloseDistanceSquared = DistanceSquared;
			CloseTarget = Actor;
		}
	}
	return CloseTarget;
}

bool UParryDetectorComponent::ExecuteParry()
{
	AActor* Target = GetParryTarget();
	if (!Target) return false;

	AActor* ActiveCharacter = GetActiveCharacter();

	// 적에게 Parried 호출
	if (Target->Implements<UCombatInteraction>())
	{
		ICombatInteraction::Execute_Parried(Target, ActiveCharacter);
		// Parry 성공 시 즉시 비활성화
		ICombatInteraction::Execute_SetParryWindowActive(Target, false);
	}

	OnParrySuccess.Broadcast(Target);
	UE_LOG(LogTemp, Log, TEXT("[ParryDetector] Parry SUCCESS on: %s"), *Target->GetName());

	return true;
}

bool UParryDetectorComponent::IsValidParryTarget(AActor* Target, const FVector& CharacterLocation, const FVector& CharacterForward) const
{
	if (!Target) return false;

	const FVector TargetLocation = Target->GetActorLocation();

	const float MaxDistanceSquared = m_fParryMaxDistance * m_fParryMaxDistance;
	const float DistanceSquared = FVector::DistSquared(CharacterLocation, TargetLocation);
	if (DistanceSquared > MaxDistanceSquared) return false;

	// 전방 각도 내의 위치에 있는지 체크
	FVector ToTarget = TargetLocation - CharacterLocation;
	ToTarget.Z = 0.f;
	ToTarget = ToTarget.GetSafeNormal();

	FVector ForwardXY = FVector(CharacterForward.X, CharacterForward.Y, 0.f).GetSafeNormal();

	const float DotProduct = FVector::DotProduct(ForwardXY, ToTarget);
	if (DotProduct < m_fParryAngle) return false;

	return true;
}

AActor* UParryDetectorComponent::GetActiveCharacter() const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return nullptr;

	return PC->GetPawn();
}

void UParryDetectorComponent::FindParryableActors(TArray<AActor*>& OutActors) const
{
	AActor* ActiveCharacter = GetActiveCharacter();
	if (!ActiveCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Origin = ActiveCharacter->GetActorLocation();
	FCollisionShape Sphere = FCollisionShape::MakeSphere(m_fParryMaxDistance);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ActiveCharacter);

	World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, Sphere, Params);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor) continue;

		if (Actor->Implements<UCombatInteraction>())
		{
			if (ICombatInteraction::Execute_CanParry(Actor))
			{
				OutActors.Add(Actor);
			}
		}
	}
}




