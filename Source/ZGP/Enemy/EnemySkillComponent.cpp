// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySkillComponent.h"
#include "EnemyAttackSelectorComponent.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"

UEnemySkillComponent::UEnemySkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemySkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UEnemySkillComponent::ExecuteAttack(FName AttackID)
{
	if (AttackID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySkillComponent] ExecuteAttack: AttackID is None"));
		return false;
	}

	const FEnemyAttackData* AttackData = FindAttackData(AttackID);
	if (!AttackData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySkillComponent] ExecuteAttack: AttackData not found for %s"), *AttackID.ToString());
		return false;
	}

	UAnimMontage* Montage = AttackData->Montage.LoadSynchronous();
	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemySkillComponent] ExecuteAttack: Montage is null for %s"), *AttackID.ToString());
		return false;
	}

	// 상태 업데이트
	m_CurrentAttackID = AttackID;
	m_pCurrentAttackData = AttackData;
	m_bIsExecuting = true;

	// 디버그 로그
	UE_LOG(LogTemp, Log, TEXT("[EnemySkillComponent] Execute Attack: %s (Type: %d, Flash: %d, Heavy: %s)"),
		*AttackID.ToString(),
		static_cast<int32>(AttackData->AttackType),
		static_cast<int32>(AttackData->FlashType),
		IsCurrentAttackHeavy() ? TEXT("Yes") : TEXT("No"));

	// 공격 시작 알림 (EnemyFlashComponent 등에서 구독)
	OnAttackStarted.Broadcast(AttackID);

	// 몽타주 재생 요청 (EnemyCharacter가 구독)
	OnRequestPlayMontage.Broadcast(Montage);

	// 쿨다운 시작 알림
	NotifyCooldownStart(AttackID);

	return true;
}

bool UEnemySkillComponent::CanExecuteAttack(FName AttackID) const
{
	if (AttackID.IsNone()) return false;

	const FEnemyAttackData* AttackData = FindAttackData(AttackID);
	if (!AttackData) return false;

	// 몽타주 유효성 검사
	if (AttackData->Montage.IsNull()) return false;

	return true;
}

bool UEnemySkillComponent::IsCurrentAttackHeavy() const
{
	if (!m_pCurrentAttackData) return false;

	// FEnemyAttackData::IsHeavyAttack() 사용
	// 판단 기준:
	// 1. bIsHeavyAttack 명시적 설정
	// 2. AttackType == MELEE_HEAVY
	// 3. HitReaction == LAUNCH
	return m_pCurrentAttackData->IsHeavyAttack();
}

bool UEnemySkillComponent::GetCurrentAttackData(FEnemyAttackData& OutData) const
{
	if (m_pCurrentAttackData)
	{
		OutData = *m_pCurrentAttackData;
		return true;
	}
	return false;
}

EAttackFlashType UEnemySkillComponent::GetCurrentFlashType() const
{
	if (m_pCurrentAttackData)
	{
		return m_pCurrentAttackData->FlashType;
	}
	return EAttackFlashType::NONE;
}

void UEnemySkillComponent::NotifyAttackCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("[EnemySkillComponent] Attack Completed: %s"), *m_CurrentAttackID.ToString());

	m_CurrentAttackID = NAME_None;
	m_pCurrentAttackData = nullptr;
	m_bIsExecuting = false;

	OnAttackCompleted.Broadcast();
}

const FEnemyAttackData* UEnemySkillComponent::FindAttackData(FName AttackID) const
{
	if (!m_pAttackDataTable) return nullptr;

	static const FString ContextString(TEXT("EnemySkillComponent_FindAttack"));
	return m_pAttackDataTable->FindRow<FEnemyAttackData>(AttackID, ContextString);
}

void UEnemySkillComponent::NotifyCooldownStart(FName AttackID)
{
	// EnemyAttackSelectorComponent에 쿨다운 시작 알림
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UEnemyAttackSelectorComponent* Selector = Owner->FindComponentByClass<UEnemyAttackSelectorComponent>();
	if (Selector)
	{
		Selector->StartAttackCooldown(AttackID);
	}
}