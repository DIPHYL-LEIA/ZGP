// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackSelectorComponent.h"
#include "../HealthComponent.h"

UEnemyAttackSelectorComponent::UEnemyAttackSelectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UEnemyAttackSelectorComponent::BeginPlay()
{
	Super::BeginPlay();

	CachePattern();
	SetComponentTickEnabled(false);		// 쿨다운 공격이 있을 때만 Tick 활성화
}

void UEnemyAttackSelectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCooldown();
}

FAttackSelectResult UEnemyAttackSelectorComponent::SelectAttackDistance(float Distance)
{
	if (!m_pCachedPattern)
	{
		CachePattern();
		if (!m_pCachedPattern)
		{
			return FAttackSelectResult();
		}
	}

	// 거리에 맞는 공격 가져오기
	const TArray<FName>& Attack = m_pCachedPattern->GetAttackByDistance(Distance);

	if (Attack.Num() == 0)
	{
		return FAttackSelectResult();
	}

	// 조건에 맞는 공격 가져오기
	float HPRatio = GetCurrentHPRatio();
	int32 Phase = GetCurrentPhase();
	TArray<FName> AvailableAttack = GetAvailableAttack(Attack, Distance, HPRatio, Phase);

	if (AvailableAttack.Num() == 0)
	{
		return FAttackSelectResult();
	}

	return SelectAttackRandom(AvailableAttack);
}

FAttackSelectResult UEnemyAttackSelectorComponent::SelectAttackRandom(const TArray<FName>& Attack)
{
	if (Attack.Num() == 0)
	{
		return FAttackSelectResult();
	}

	// 가중치를 기반으로 랜덤 선택
	FName SelectedID = SelectAttackWeight(Attack);

	if (SelectedID.IsNone())
	{
		return FAttackSelectResult();
	}

	// 연속 같은 공격 방지
	if (m_bPreventRepeatAttack && SelectedID == m_LastSelectAttackID)
	{
		m_nRepeatCount++;

		if (m_nRepeatCount >= m_nMaxRepeatCount)
		{
			// 다른 공격 시도
			TArray<FName> OtherAttack = Attack;
			OtherAttack.Remove(SelectedID);

			if (OtherAttack.Num() > 0)
			{
				SelectedID = SelectAttackWeight(OtherAttack);
				m_nRepeatCount = 0;
			}
		}
	}
	else
	{
		m_nRepeatCount = 0;
	}

	const FEnemyAttackData* AttackData = FindAttackData(SelectedID);
	if (!AttackData)
	{
		return FAttackSelectResult();
	}

	// 마지막 선택 기록
	m_LastSelectAttackID = SelectedID;

	OnAttackSelect.Broadcast(SelectedID, *AttackData);

	return FAttackSelectResult(true, SelectedID, *AttackData);
}

FAttackSelectResult UEnemyAttackSelectorComponent::SelectAttackPhase(int32 CurrentPhase, float Distance)
{
	if (!m_pCachedPattern)
	{
		CachePattern();
		if (!m_pCachedPattern)
		{
			return FAttackSelectResult();
		}
	}

	// 현재 페이즈에 맞는 공격 가져오기
	const TArray<FName>& PhaseAttack = m_pCachedPattern->GetAttackPhase(CurrentPhase);

	// 페이즈 없으면 거리 기반
	if (PhaseAttack.Num() == 0)
	{
		return SelectAttackDistance(Distance);
	}

	float HPRatio = GetCurrentHPRatio();
	TArray<FName> AvailableAttack = GetAvailableAttack(PhaseAttack, Distance, HPRatio, CurrentPhase);

	if (AvailableAttack.Num() == 0)
	{
		return SelectAttackDistance(Distance);
	}
	return SelectAttackRandom(AvailableAttack);
}

FAttackSelectResult UEnemyAttackSelectorComponent::SelectAttackID(FName AttackID)
{
	if (AttackID.IsNone())
	{
		return FAttackSelectResult();
	}

	const FEnemyAttackData* AttackData = FindAttackData(AttackID);
	if (!AttackData)
	{
		return FAttackSelectResult();
	}

	if (!IsAttackReady(AttackID))
	{
		return FAttackSelectResult();
	}

	m_LastSelectAttackID = AttackID;
	OnAttackSelect.Broadcast(AttackID, *AttackData);

	return FAttackSelectResult(true, AttackID, *AttackData);
}

FAttackSelectResult UEnemyAttackSelectorComponent::SelectAttackNextCombo(FName CurrentAttackID)
{
	if (CurrentAttackID.IsNone())
	{
		return FAttackSelectResult();
	}

	const FEnemyAttackData* CurrentAttack = FindAttackData(CurrentAttackID);
	if (!CurrentAttack)
	{
		return FAttackSelectResult();
	}

	if (CurrentAttack->NextComboID.IsNone())
	{
		return FAttackSelectResult();
	}

	// 콤보 체인 확률 체크
	if (CurrentAttack->ComboChainChance < 1.0f)
	{
		float RandomValue = FMath::FRand();
		if (RandomValue > CurrentAttack->ComboChainChance)
		{
			return FAttackSelectResult();
		}
	}

	return SelectAttackID(CurrentAttack->NextComboID);
}

void UEnemyAttackSelectorComponent::StartAttackCooldown(FName AttackID)
{
	if (AttackID.IsNone()) return;

	const FEnemyAttackData* AttackData = FindAttackData(AttackID);
	if (!AttackData || AttackData->Cooldown <= 0.f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	float CurrentTime = World->GetTimeSeconds();
	float CooldownEndTime = CurrentTime + AttackData->Cooldown;

	m_mapCooldownEnd.Add(AttackID, CooldownEndTime);

	SetComponentTickEnabled(true);
}

bool UEnemyAttackSelectorComponent::IsAttackReady(FName AttackID) const
{
	if (AttackID.IsNone()) return false;

	// 쿨다운 기록이 없으면 사용 가능
	const float* CooldownEndTime = m_mapCooldownEnd.Find(AttackID);		// ??
	if (!CooldownEndTime) return true;

	UWorld* World = GetWorld();
	if (!World) return false;

	return World->GetTimeSeconds() >= *CooldownEndTime;
}

float UEnemyAttackSelectorComponent::GetRemainingCooldown(FName AttackID) const
{
	if (AttackID.IsNone()) return 0.f;

	const float* CooldownEndTime = m_mapCooldownEnd.Find(AttackID);
	if (!CooldownEndTime) return 0.f;

	UWorld* World = GetWorld();
	if (!World) return false;

	float Remaining = *CooldownEndTime - World->GetTimeSeconds();

	return FMath::Max(0.f, Remaining);
}

void UEnemyAttackSelectorComponent::ResetAllCooldown()
{
	m_mapCooldownEnd.Empty();
	SetComponentTickEnabled(false);
}

bool UEnemyAttackSelectorComponent::GetAttackData(FName AttackID, FEnemyAttackData& Data) const
{
	const FEnemyAttackData* AttackData = FindAttackData(AttackID);
	if (!AttackData) return false;

	Data = *AttackData;
	return true;
}

bool UEnemyAttackSelectorComponent::GetPattern(FEnemyAttackPattern& Pattern) const
{
	if (!m_pCachedPattern) return false;

	Pattern = *m_pCachedPattern;
	return true;
}

TArray<FName> UEnemyAttackSelectorComponent::GetAttackDistance(float Distance) const
{
	if (!m_pCachedPattern)
	{
		return TArray<FName>();
	}
	return m_pCachedPattern->GetAttackByDistance(Distance);
}

TArray<FName> UEnemyAttackSelectorComponent::GetAvailableAttack(const TArray<FName>& Attack, float Distance, float HPRatio, int32 Phase) const
{
	TArray<FName> Result;
	Result.Reserve(Attack.Num());

	for (const FName& AttackID : Attack)
	{
		if (CanUseAttack(AttackID, Distance, HPRatio, Phase))
		{
			Result.Add(AttackID);
		}
	}
	return Result;
}

int32 UEnemyAttackSelectorComponent::GetCurrentPhase() const
{
	float HPRatio = GetCurrentHPRatio();
	return FEnemyAttackPattern::CalculatePhase(HPRatio);;
}

float UEnemyAttackSelectorComponent::GetCurrentHPRatio() const
{
	return SetHPRatio();
}

void UEnemyAttackSelectorComponent::CachePattern()
{
	m_pCachedPattern = nullptr;

	if (!m_pPatternTable || m_PatternRowName.IsNone()) return;

	static const FString ContextString(TEXT("EnemyAttackSelectorComponent"));
	m_pCachedPattern = m_pPatternTable->FindRow<FEnemyAttackPattern>(m_PatternRowName, ContextString);
}

const FEnemyAttackData* UEnemyAttackSelectorComponent::FindAttackData(FName AttackID) const
{
	if (!m_pAttackDataTable || AttackID.IsNone()) return nullptr;

	static const FString ContextString(TEXT("EnemyAttackSelectorComponent"));
	return m_pAttackDataTable->FindRow<FEnemyAttackData>(AttackID, ContextString);
}

FName UEnemyAttackSelectorComponent::SelectAttackWeight(const TArray<FName>& AttackIDs) const
{
	if (AttackIDs.Num() == 0) return NAME_None;
	if (AttackIDs.Num() == 1) return AttackIDs[0];

	float TotalWeight = 0.f;
	TArray<float> Weights;
	Weights.Reserve(AttackIDs.Num());

	for (const FName& AttackID : AttackIDs)
	{
		const FEnemyAttackData* Data = FindAttackData(AttackID);
		float Weight = Data ? Data->SelectWeight : 1.0f;
		Weights.Add(Weight);
		TotalWeight += Weight;
	}

	// 가중치 0이면 랜덤
	if (TotalWeight <= 0.f)
	{
		int32 RandomIndex = FMath::RandRange(0, AttackIDs.Num() - 1);
		return AttackIDs[RandomIndex];
	}

	float RandomValue = FMath::FRandRange(0.f, TotalWeight);
	float AccumulateWeight = 0.f;

	for (int32 i = 0; i < AttackIDs.Num(); ++i)
	{
		AccumulateWeight += Weights[i];
		if (RandomValue <= AccumulateWeight)
		{
			return AttackIDs[i];
		}
	}

	// 선택되지 않은 경우 마지막 거 선택
	return AttackIDs.Last();
}

bool UEnemyAttackSelectorComponent::CanUseAttack(FName AttackID, float Distance, float HPRatio, int32 Phase) const
{
	if (!IsAttackReady(AttackID)) return false;

	const FEnemyAttackData* Data = FindAttackData(AttackID);
	if (!Data) return false;

	if (Data->RangeZone != EAttackRangeZone::ANY)
	{
		if (!Data->IsInRange(Distance)) return false;
	}

	if (!Data->IsHPCondition(HPRatio)) return false;
	if (!Data->IsPhaseCondition(Phase)) return false;

	return true;
}

void UEnemyAttackSelectorComponent::UpdateCooldown()
{
	if (m_mapCooldownEnd.Num() == 0)
	{
		SetComponentTickEnabled(false);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	float CurrentTime = World->GetTimeSeconds();
	TArray<FName> CompleteCooldowns;

	for (const auto& Pair : m_mapCooldownEnd)
	{
		if (CurrentTime >= Pair.Value)
		{
			CompleteCooldowns.Add(Pair.Key);
		}
	}

	for (const FName& AttackID : CompleteCooldowns)
	{
		m_mapCooldownEnd.Remove(AttackID);
		OnAttackCooldownComplete.Broadcast(AttackID);
	}

	// 쿨다운 모두 완료되면 Tick 비활성화
	if (m_mapCooldownEnd.Num() == 0)
	{
		SetComponentTickEnabled(false);
	}

}

float UEnemyAttackSelectorComponent::SetHPRatio() const
{
	AActor* Owner = GetOwner();
	if (!Owner) return 1.0f;

	UHealthComponent* HealthComponent = Owner->FindComponentByClass<UHealthComponent>();
	if (!HealthComponent) return 1.0f;

	float MaxHP = HealthComponent->GetMaxHealth();
	if (MaxHP <= 0.f) return 1.0f;


	return 0.0f;
}


