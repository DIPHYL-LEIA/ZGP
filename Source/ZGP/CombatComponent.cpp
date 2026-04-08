// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "CombatInteraction.h"
#include "ResourceProvider.h"
#include "DamageData.h"
#include "DrawDebugHelpers.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginAttack()
{
	// 새 공격 모션 시작 시 리스트 초기화
	m_setHitActors.Empty();
}

void UCombatComponent::EndAttack()
{
	m_setHitActors.Empty();
}

void UCombatComponent::PerformAttackTrace(const FAttackData& AttackData)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const FVector OwnerRight = Owner->GetActorRightVector();
	const FVector OwnerUp = Owner->GetActorUpVector();

	// Owner Local -> World로 오프셋 적용
	FVector OffsetWorld = (OwnerForward * AttackData.Offset.X) + (OwnerRight * AttackData.Offset.Y) + (OwnerUp * AttackData.Offset.Z);

	FVector TraceStart = OwnerLocation + OffsetWorld;
	FVector TraceEnd = TraceStart + (OwnerForward * AttackData.Range);

	// Trace
	TArray<FHitResult> HitResult;
	ExecuteTrace(TraceStart, TraceEnd, AttackData.Radius, HitResult);

	// 타격 모션 당 1회 Resource 축적
	bool bHasGrantedResource = false;

	for (const FHitResult& Hit : HitResult)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;
		if (HitActor == Owner) continue;

		if (!AttackData.bAllowMultiHit)
		{
			if (m_setHitActors.Contains(HitActor))
			{
				continue;
			}
			m_setHitActors.Add(HitActor);
		}

		ApplyDamage(HitActor, AttackData, Hit);

		// Add Resource
		if (!bHasGrantedResource && Owner->Implements<UResourceProvider>())
		{
			IResourceProvider::Execute_AddResource(Owner, AttackData.ResourceGain, AttackData.DecibelGain);
			bHasGrantedResource = true;

			UE_LOG(LogTemp, Warning, TEXT("[CombatComp] Resource Granted - Energy: %.1f, Decibel: %.1f"),
				AttackData.ResourceGain, AttackData.DecibelGain);
		}

		FVector HitDirection = (HitActor->GetActorLocation() - OwnerLocation).GetSafeNormal();
		OnAttackHit.Broadcast(HitActor, Hit.ImpactPoint, HitDirection);
	}

	/////////////////////////// 디버그 ///////////////////////
	if (m_bShowDebug)
	{
		DrawDebugCapsule(GetWorld(), (TraceStart + TraceEnd) * 0.5f,
			AttackData.Range * 0.5f, AttackData.Radius,
			FQuat::FindBetweenVectors(FVector::UpVector, OwnerForward),
			HitResult.Num() > 0 ? FColor::Red : FColor::Green,
			false, 0.5f);
	}

}


void UCombatComponent::ExecuteTrace(const FVector& Start, const FVector& End, float Radius, TArray<FHitResult>& OutHits)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	World->SweepMultiByChannel(OutHits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(Radius), Params);

}

void UCombatComponent::ApplyDamage(AActor* Target, const FAttackData& AttackData, const FHitResult& HitResult)
{
	if (!Target) return;

	UE_LOG(LogTemp, Warning, TEXT("[CombatComp] ApplyDamage Target: %s, Class: %s"),
		*Target->GetName(), *Target->GetClass()->GetName());

	if (!Target->Implements<UCombatInteraction>()) return;

	AActor* Owner = GetOwner();

	FDamageData DamageData;
	DamageData.BaseDamageValue = AttackData.DamageMultiply * 10.f;		// 나중에 스탯과 연동 필요
	DamageData.DazeValue = AttackData.DazeValue;
	DamageData.Attacker = Owner;
	DamageData.HitLocation = HitResult.ImpactPoint;
	DamageData.HitDirection = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
	DamageData.HitReaction = AttackData.HitReaction;
	DamageData.bIsHeavyAttack = AttackData.bIsHeavyAttack;

	ICombatInteraction::Execute_ApplyCombatEffect(Target, DamageData);
}
