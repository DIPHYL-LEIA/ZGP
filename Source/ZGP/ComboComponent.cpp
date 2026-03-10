// Fill out your copyright notice in the Description page of Project Settings.

#include "ComboComponent.h"
#include "ActionStateProvider.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"


UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadComboData();

}

void UComboComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearResetTimer();
	Super::EndPlay(EndPlayReason);
}

void UComboComponent::OnAttackPressed()
{
	m_bIsAttackPressed = true;
	m_fAttackPressTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
}

void UComboComponent::OnAttackReleased()
{
	if (!m_bIsAttackPressed) return;

	m_bIsAttackPressed = false;

	float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	float HoldDuration = CurrentTime - m_fAttackPressTime;
	float HoldTime = GetCurrentHoldTime();

	EInputAttackType InputType = (HoldDuration >= HoldTime) ? EInputAttackType::HOLD : EInputAttackType::TAP;

	if (m_bIsInComboWindow)
	{
		// Window 안일 때 즉시 실행
		ExecuteCombo(InputType);
	}
	else if (m_bIsComboActive)
	{
		// Window 전 버퍼에 저장, 마지막 콤보 아닐 때만 선입력 허용
		if (!m_bIsLastCombo)
		{
			m_bPendingInput = true;
			m_ePendingInputType = InputType;
		}
	}
	else
	{
		// Idle일 때 콤보 시작
		ExecuteCombo(InputType);
	}
}

void UComboComponent::RequestComboAttack()
{
	if (m_bIsInComboWindow)
	{
		ExecuteCombo(EInputAttackType::TAP);
	}
	else if (m_bIsComboActive)
	{
		if (!m_bIsLastCombo)
		{
			m_bPendingInput = true;
			m_ePendingInputType = EInputAttackType::TAP;
		}
	}
	else
	{
		ExecuteCombo(EInputAttackType::TAP);
	}
}

void UComboComponent::StartComboWindow()
{
	m_bIsInComboWindow = true;
	ClearResetTimer();

	if (m_bPendingInput)
	{
		m_bPendingInput = false;
		ExecuteCombo(m_ePendingInputType);
	}
}

void UComboComponent::EndComboWindow()
{
	m_bIsInComboWindow = false;

	if (m_bIsComboActive)
	{
		StartResetTimer();
		//if (m_bIsLastCombo)
		//{
		//	StartResetTimer();
		//}
		//else
		//{
		//	StartResetTimer();
		//}
	}
}

void UComboComponent::ResetCombo()
{
	ClearResetTimer();

	int32 PrevIndex = m_nCurrentComboIndex;

	m_nCurrentComboIndex = 0;
	m_bIsInComboWindow = false;
	m_bIsComboActive = false;
	m_bPendingInput = false;
	m_bIsLastCombo = false;

	m_bIsAttackPressed = false;

	if (PrevIndex > 0)
	{
		OnResetCombo.Broadcast();
	}
}

void UComboComponent::LoadComboData()
{
	m_pCachedComboData = nullptr;

	if (!m_pComboDataTable || m_ComboDataRowName.IsNone()) return;

	static const FString ContextString(TEXT("ComboComponent"));
	m_pCachedComboData = m_pComboDataTable->FindRow<FComboData>(m_ComboDataRowName, ContextString);
}

void UComboComponent::ExecuteCombo(EInputAttackType InputType)
{
	ClearResetTimer();

	if (!m_pCachedComboData || m_pCachedComboData->IsEmpty()) return;

	int32 NextIndex = 0;

	if (m_bIsComboActive)
	{
		NextIndex = m_pCachedComboData->GetNextComboIndex(m_nCurrentComboIndex, InputType);

		if (NextIndex < 0 || !m_pCachedComboData->ComboChain.IsValidIndex(NextIndex))
		{
			ResetCombo();
			NextIndex = 0;
		}
	}

	const FComboNode* Node = m_pCachedComboData->GetComboNode(NextIndex);
	if (!Node)
	{
		ResetCombo();
		return;
	}

	m_nCurrentComboIndex = NextIndex;
	m_bIsComboActive = true;
	m_bIsInComboWindow = false;
	m_bIsLastCombo = Node->bIsLastCombo;

	UE_LOG(LogTemp, Log, TEXT("[ComboComponent] Execute Combo Index: %d, SkillID: %s, IsLast: %s"),
		NextIndex, *Node->SkillID.ToString(), Node->bIsLastCombo ? TEXT("Yes") : TEXT("No"));

	OnPerformComboAttack.Broadcast(NextIndex, Node->bIsLastCombo);
}

float UComboComponent::GetCurrentHoldTime() const
{
	if (m_pCachedComboData && m_bIsComboActive)
	{
		const FComboNode* Node = m_pCachedComboData->GetComboNode(m_nCurrentComboIndex);
		if (Node && Node->bIsHold)
		{
			return Node->HoldTime;
		}
	}
	return m_fDefaultHoldTime;
}

void UComboComponent::StartResetTimer()
{
	ClearResetTimer();

	UWorld* World = GetWorld();
	if (!World) return;

	float ResetTimer = m_pCachedComboData ? m_pCachedComboData->ComboResetTime : 1.5f;

	World->GetTimerManager().SetTimer(ComboResetTimerHandle, this, &UComboComponent::OnResetTimerExpired, ResetTimer, false);
}

void UComboComponent::ClearResetTimer()
{
	if (ComboResetTimerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(ComboResetTimerHandle);
		}
		ComboResetTimerHandle.Invalidate();
	}
}

void UComboComponent::OnResetTimerExpired()
{
	ComboResetTimerHandle.Invalidate();
	ResetCombo();
}
