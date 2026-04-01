// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"
#include "Engine/DataTable.h"
#include "ComboData.h"
#include "Animation/AnimMontage.h"
#include "ResourceProvider.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USkillComponent::ExecuteComboAttack(int32 ComboIndex, bool bIsLastcombo)
{
	FName SkillID = GetSkillIDFromComboIndex(ComboIndex);

	if (SkillID.IsNone()) return;

	// 마지막 콤보는 Heavy Attack 처리
	ExecuteSkillInternal(SkillID, true, bIsLastcombo);
}

bool USkillComponent::ExecuteSkillID(FName SkillID)
{
	return ExecuteSkillInternal(SkillID, false, false);
}

bool USkillComponent::CanExecuteSkill(FName SkillID) const
{
	const FSkillData* SkillData = FindSkillData(SkillID);
	if (!SkillData) return false;

	// Check Resource
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UResourceProvider>())
	{
		if (!IResourceProvider::Execute_HasResource(Owner, SkillData->EnergyCost, SkillData->DecibelCost))
		{
			return false;
		}
	}

	return true;
}

bool USkillComponent::IsCurrentSkillHeavy() const
{
	if (m_pCurrentSkillData && m_pCurrentSkillData->bIsHeavyAttack) return true;
	if (m_bIsCurrentLastCombo) return true;
	
	return false;
}

void USkillComponent::NotifyHeavyAttackHit(AActor* Target)
{
	if (!Target) return;

	OnHeavyAttackHit.Broadcast(Target);
}

bool USkillComponent::GetCurrentSkillDataBP(FSkillData& Data) const
{
	if (m_pCurrentSkillData)
	{
		Data = *m_pCurrentSkillData;
		return true;
	}

	return false;
}

ESkillType USkillComponent::GetCurrentSkillType() const
{
	if (m_pCurrentSkillData)
	{
		return m_pCurrentSkillData->SkillType;
	}
	return ESkillType::NONE;
}

void USkillComponent::NotifySkillCompleted()
{
	m_CurrentSkillID = NAME_None;
	m_pCurrentSkillData = nullptr;
	m_bIsCurrentLastCombo = false;

	OnSkillExecuteCompleted.Broadcast();
}

bool USkillComponent::ExecuteSkillInternal(FName SkillID, bool bOverrideHeavy, bool bIsHeavyOverride)
{
	if (SkillID.IsNone()) return false;

	const FSkillData* SkillData = FindSkillData(SkillID);
	if (!SkillData) return false;

	// Check Resource
	AActor* Owner = GetOwner();
	if (Owner && Owner->Implements<UResourceProvider>())
	{
		if (!IResourceProvider::Execute_HasResource(Owner, SkillData->EnergyCost, SkillData->DecibelCost))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] Not enough resource for: %s"), *SkillID.ToString());
			return false;
		}

		// Consume
		IResourceProvider::Execute_ConsumeResource(Owner, SkillData->EnergyCost, SkillData->DecibelCost);
	}

	UAnimMontage* Montage = SkillData->Montage.LoadSynchronous();
	if (!Montage) return false;

	m_CurrentSkillID = SkillID;
	m_pCurrentSkillData = SkillData;
	m_bIsCurrentLastCombo = bOverrideHeavy && bIsHeavyOverride;

	// ------------------디버그----------------------
	UE_LOG(LogTemp, Log, TEXT("[SkillComponent] Execute Skill: %s (Type: %d, LastCombo: %s)"),
		*SkillID.ToString(),
		static_cast<int32>(SkillData->SkillType),
		m_bIsCurrentLastCombo ? TEXT("Yes") : TEXT("No"));

	OnRequestPlayMontage.Broadcast(Montage);

	return true;
}

FName USkillComponent::GetSkillIDFromComboIndex(int32 ComboIndex) const
{
	if (!m_pComboDataTable || m_ComboDataRowName.IsNone()) return NAME_None;

	static const FString ContextString(TEXT("SkillComponent_GetSkillID"));
	const FComboData* ComboData = m_pComboDataTable->FindRow<FComboData>(m_ComboDataRowName, ContextString);
	if (!ComboData) return NAME_None;

	const FComboNode* Node = ComboData->GetComboNode(ComboIndex);
	if (!Node) return NAME_None;

	return Node->SkillID;
}

const FSkillData* USkillComponent::FindSkillData(FName SkillID) const
{
	if (!m_pSkillDataTable) return nullptr;

	static const FString ContextString(TEXT("SkillComponent_FindSkill"));
	return m_pSkillDataTable->FindRow<FSkillData>(SkillID, ContextString);
}



