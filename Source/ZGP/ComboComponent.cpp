// Fill out your copyright notice in the Description page of Project Settings.

#include "ComboComponent.h"
#include "ActionStateProvider.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"


UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//bWantsInitializeComponent = true;

	m_nCurrentComboStep = 0;
	m_bIsComboWindowStart = false;
	m_fComboResetTime = 1.3f;
}

void UComboComponent::BeginPlay()
{
	// 생성 시점 확인 필요
	AActor* Owner = GetOwner();
	if (Owner)
	{
		ActionStateProvider = Cast<IActionStateProvider>(Owner);
	}
}

void UComboComponent::RequestComboAttack()
{
	if (ActionStateProvider == nullptr) return;

	// Combo Start (0 -> 1)
	if (m_nCurrentComboStep == 0)
	{
		if (ActionStateProvider->CanChangeActionState(EActionState::ATTACKING))
		{
			m_nCurrentComboStep = 1;
			m_bIsComboWindowStart = false;

			ActionStateProvider->SetActionState(EActionState::ATTACKING);

			// Skill Component에 Broadcast
			OnPerformComboAttack.Broadcast(m_nCurrentComboStep);
		}

		// Combo Reset Timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ComboResetTimer, this, &UComboComponent::HandleComboReset, m_fComboResetTime, false);
		}
	}
	// Combo Chain (1 -> 2 , 2 -> 3)
	else if (m_bIsComboWindowStart)
	{
		m_nCurrentComboStep++;
		m_bIsComboWindowStart = false;

		// Skill Component에 N단계 공격 실행
		OnPerformComboAttack.Broadcast(m_nCurrentComboStep);

		// Combo Reset Timer
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ComboResetTimer, this, &UComboComponent::HandleComboReset, m_fComboResetTime, false);
		}
	}
}

void UComboComponent::StartComboWindow()
{
	if (m_nCurrentComboStep > 0)
	{
		m_bIsComboWindowStart = true;
		UE_LOG(LogTemp, Log, TEXT("Combo Window Open (Step : %d)"), m_nCurrentComboStep);
	}
}

void UComboComponent::ResetCombo()
{
	UE_LOG(LogTemp, Log, TEXT("Last Combo Step (Step : %d)"), m_nCurrentComboStep);

	m_nCurrentComboStep = 0;
	m_bIsComboWindowStart = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ComboResetTimer);
	}

	if (ActionStateProvider && ActionStateProvider->IsActionState(EActionState::ATTACKING))
	{
		if (ActionStateProvider->CanChangeActionState(EActionState::IDLE))
		{
			ActionStateProvider->SetActionState(EActionState::IDLE);
		}
	}
}

void UComboComponent::HandleComboReset()
{
	ResetCombo();
}

