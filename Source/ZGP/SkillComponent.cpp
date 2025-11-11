// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"
#include "GameFramework/Actor.h"	// GetOwner()
#include "Engine/DataTable.h"
#include "SkillData.h"
#include "ComboData.h"
#include "Animation/AnimMontage.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;	// ActionStateComponent보다 늦게 실행?
}

void USkillComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (m_pSkillDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillComponent::InitializeComp - NO Skill DataTable"));
	}
	if (m_pComboDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillComponent::InitializeComp - NO Combo DataTable"));
	}

}

void USkillComponent::ExecuteComboAttack(int32 ComboStep)
{
	if (m_pSkillDataTable == nullptr || m_pComboDataTable == nullptr) return;
	if (ComboStep <= 0) return;

	const FComboData* pCombo = m_pComboDataTable->FindRow<FComboData>(m_currentComboID, TEXT("ExecuteComboAttack"));
	if (pCombo == nullptr) return;

	if (pCombo->SkillID.IsValidIndex(ComboStep - 1) == false) return;

	const FName SkillId = pCombo->SkillID[ComboStep - 1];

	const FSkillData* pSkillData = m_pSkillDataTable->FindRow<FSkillData>(SkillId, TEXT("ExecuteComboAttack"));
	if (pSkillData == nullptr) return;

	// 즉각 공격 반응을 위해 동기 로드 사용
	UAnimMontage* pMontagePlay = pSkillData->Montage.LoadSynchronous();

	if (pMontagePlay)
	{
		OnRequestPlayMontage.Broadcast(pMontagePlay);
	}

}



