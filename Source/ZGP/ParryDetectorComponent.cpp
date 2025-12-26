// Fill out your copyright notice in the Description page of Project Settings.


#include "ParryDetectorComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "CombatInteraction.h"

UParryDetectorComponent::UParryDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UParryDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UParryDetectorComponent::RegisterParryableAttack(AActor* Attacker)
{
	if (!Attacker) return;

	CleanExpiredAttack();

	UWorld* World = GetWorld();
	if (!World) return;

	float CurrentTime = World->GetTimeSeconds();

	// 이미 등록된 공격자인지 확인
	for (FParryableAttackInfo& Info : m_arParryableAttacks)
	{
		if (Info.attacker.Get() == Attacker)
		{
			Info.registerTime = CurrentTime;
			return;
		}
	}

	m_arParryableAttacks.Add(FParryableAttackInfo(Attacker, CurrentTime));

}

void UParryDetectorComponent::UnregisterParryableAttack(AActor* Attacker)
{
	if (!Attacker) return;

	// ???
	for (int32 i = m_arParryableAttacks.Num() - 1; i >= 0; --i)
	{
		if (m_arParryableAttacks[i].attacker.Get() == Attacker)
		{
			m_arParryableAttacks.RemoveAt(i);
			return;
		}
	}
}

bool UParryDetectorComponent::CanParry() const
{
	if (m_arParryableAttacks.Num() == 0) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	AActor* ActiveCharacter = GetActiveCharacter();
	if (!ActiveCharacter) return false;

	const float CurrentTime = World->GetTimeSeconds();
	const FVector Location = ActiveCharacter->GetActorLocation();
	const FVector Forward = ActiveCharacter->GetActorForwardVector();

	for (const FParryableAttackInfo& Info : m_arParryableAttacks)
	{
		if (IsValidParryTarget(Info, Location, Forward, CurrentTime))
		{
			return true;
		}
	}

	return false;
}

AActor* UParryDetectorComponent::GetParryTarget() const
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	AActor* ActiveCharacter = GetActiveCharacter();
	if (!ActiveCharacter) return nullptr;

	const float CurrentTime = World->GetTimeSeconds();
	const FVector Location = ActiveCharacter->GetActorLocation();
	const FVector Forward = ActiveCharacter->GetActorForwardVector();
	const float MaxDistanceSquared = m_fParryMaxDistane * m_fParryMaxDistane;

	// 가까운 타겟
	AActor* CloseTarget = nullptr;
	float CloseDistanceSquared = MaxDistanceSquared;

	for (const FParryableAttackInfo& Info : m_arParryableAttacks)
	{
		if (!IsValidParryTarget(Info, Location, Forward, CurrentTime))
		{
			continue;
		}

		float DistanceSquared = FVector::DistSquared(Location, Info.attacker->GetActorLocation());

		if (DistanceSquared < CloseDistanceSquared)
		{
			CloseDistanceSquared = DistanceSquared;
			CloseTarget = Info.attacker.Get();
		}
	}
	return CloseTarget;
}

bool UParryDetectorComponent::ExecuteParry()
{
	CleanExpiredAttack();

	AActor* Target = GetParryTarget();
	if (!Target) return false;

	// 적에게 Parried 호출
	if (Target->Implements<UCombatInteraction>())
	{
		AActor* ActiveCharacter = GetActiveCharacter();
		ICombatInteraction::Execute_Parried(Target, ActiveCharacter);
	}

	UnregisterParryableAttack(Target);

	OnParrySuccess.Broadcast(Target);
	UE_LOG(LogTemp, Log, TEXT("[ParryDetector] Parry SUCCESS on: %s"), *Target->GetName());

	return true;
}

void UParryDetectorComponent::CleanExpiredAttack()
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float CurrentTime = World->GetTimeSeconds();

	for (int32 i = m_arParryableAttacks.Num() - 1; i >= 0; --i)
	{
		const FParryableAttackInfo& Info = m_arParryableAttacks[i];

		if (!Info.attacker.IsValid() || CurrentTime - Info.registerTime > m_fParryWindowDuration)
		{
			m_arParryableAttacks.RemoveAt(i);
		}
	}
}

bool UParryDetectorComponent::IsValidParryTarget(const FParryableAttackInfo& Info, const FVector& CharacterLocation, const FVector& CharacterForward, float CurrentTime) const
{
	if (!Info.attacker.IsValid()) return false;

	if (CurrentTime - Info.registerTime > m_fParryWindowDuration) return false;

	const FVector AttackerLocation = Info.attacker->GetActorLocation();

	const float MaxDistanceSquared = m_fParryMaxDistane * m_fParryMaxDistane;
	const float DistanceSquared = FVector::DistSquared(CharacterLocation, AttackerLocation);
	if (DistanceSquared > MaxDistanceSquared) return false;

	// 전방 각도 내의 위치에 있는지 체크
	FVector ToAttacker = AttackerLocation - CharacterLocation;
	ToAttacker.Z = 0.f;
	ToAttacker = ToAttacker.GetSafeNormal();

	FVector ForwardXY = FVector(CharacterForward.X, CharacterForward.Y, 0.f).GetSafeNormal();

	const float DotProduct = FVector::DotProduct(ForwardXY, ToAttacker);
	if (DotProduct < m_fParryAngle) return false;

	return true;
}

AActor* UParryDetectorComponent::GetActiveCharacter() const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return nullptr;

	return PC->GetPawn();
}




