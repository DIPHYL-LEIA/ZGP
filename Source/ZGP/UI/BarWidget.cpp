// Fill out your copyright notice in the Description page of Project Settings.


#include "BarWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/App.h"

const FName UBarWidget::PARAM_PERCENT = FName("Percent");
const FName UBarWidget::PARAM_COLOR = FName("BarColor");

UBarWidget::UBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeMaterials();

	m_fCurrentPercent = 1.0f;
	m_fTargetPercent = 1.0f;
	m_fGhostPercent = 1.0f;
	m_bIsTickEnabled = false;


	//// 값이 변할 때만 Tick 켜기

	//if (CurrentBar)
	//{
	//	CurrentBar->SetPercent(m_fCurrentPercent);
	//}
	//if (GhostBar)
	//{
	//	GhostBar->SetPercent(m_fGhostPercent);
	//}
}

void UBarWidget::NativeDestruct()
{
	EnableTick(false);

	m_pCurrentBarID = nullptr;
	m_pGhostBarID = nullptr;

	Super::NativeDestruct();

}

void UBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!m_bIsTickEnabled) return;

	const float DeltaTime = GetDeltaTime(InDeltaTime);
	UpdateGhostBar(DeltaTime);
}

void UBarWidget::InitializeMaterials()
{
	if (!m_pBarMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ZGPBarWidget] Bar Material is not set"));
		return;
	}

	// Current Bar Material Instance
	if (CurrentBar)
	{
		m_pCurrentBarID = UMaterialInstanceDynamic::Create(m_pBarMaterial, this);
		if (m_pCurrentBarID)
		{
			m_pCurrentBarID->SetScalarParameterValue(PARAM_PERCENT, m_fCurrentPercent);
			m_pCurrentBarID->SetVectorParameterValue(PARAM_COLOR, m_CurrentBarColor);

			FSlateBrush Brush;
			Brush.SetResourceObject(m_pCurrentBarID);
			CurrentBar->SetBrush(Brush);
		}
	}

	// Ghost Bar Material Instance
	if (GhostBar)
	{
		m_pGhostBarID = UMaterialInstanceDynamic::Create(m_pBarMaterial, this);
		if (m_pGhostBarID)
		{
			m_pGhostBarID->SetScalarParameterValue(PARAM_PERCENT, m_fGhostPercent);
			m_pGhostBarID->SetVectorParameterValue(PARAM_COLOR, m_GhostBarColor);

			FSlateBrush Brush;
			Brush.SetResourceObject(m_pGhostBarID);
			GhostBar->SetBrush(Brush);
		}
	}
}

void UBarWidget::SetPercent(float NewPercent)
{
	NewPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);

	if (FMath::IsNearlyEqual(m_fTargetPercent, NewPercent)) return;

	const float OldTargetPercent = m_fTargetPercent;
	m_fTargetPercent = NewPercent;

	// CurrentBar 세팅
	m_fCurrentPercent = NewPercent;
	if (m_pCurrentBarID)
	{
		m_pCurrentBarID->SetScalarParameterValue(PARAM_PERCENT, m_fCurrentPercent);
	}

	// Ghost 동작 결정
	const bool bIsDecrease = (NewPercent < OldTargetPercent);
	const bool bShouldGhostFollow = m_bInvertGhost ? !bIsDecrease : bIsDecrease;

	if (bShouldGhostFollow)
	{
		m_bGhostDelayActive = true;
		m_fGhostDelayTimer = m_fGhostDelay;
		EnableTick(true);
	}
	else
	{
		m_fGhostPercent = NewPercent;
		if (m_pGhostBarID)
		{
			m_pGhostBarID->SetScalarParameterValue(PARAM_PERCENT, m_fGhostPercent);
		}

		if (FMath::IsNearlyEqual(m_fGhostPercent, m_fCurrentPercent, m_fGhostTolerance))
		{
			EnableTick(false);
		}
	}
}

void UBarWidget::SetPercentNow(float NewPercent)
{
	NewPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);

	m_fTargetPercent = NewPercent;
	m_fCurrentPercent = NewPercent;
	m_fGhostPercent = NewPercent;

	m_bGhostDelayActive = false;
	m_fGhostDelayTimer = 0.0f;

	if (m_pCurrentBarID)
	{
		m_pCurrentBarID->SetScalarParameterValue(PARAM_PERCENT, m_fCurrentPercent);
	}
	if (m_pGhostBarID)
	{
		m_pGhostBarID->SetScalarParameterValue(PARAM_PERCENT, m_fGhostPercent);
	}
	
	EnableTick(false);
}

void UBarWidget::SetUseRealTime(bool bUseRealTime)
{
	m_bUseRealTime = bUseRealTime;
}

void UBarWidget::SetBarColor(FLinearColor NewColor)
{
	m_CurrentBarColor = NewColor;
	if (m_pCurrentBarID)
	{
		m_pCurrentBarID->SetVectorParameterValue(PARAM_COLOR, NewColor);
	}
}


void UBarWidget::EnableTick(bool bEnable)
{
	if (m_bIsTickEnabled == bEnable) return;
	m_bIsTickEnabled = bEnable;
}

void UBarWidget::UpdateGhostBar(float DeltaTime)
{
	// 딜레이 처리
	if (m_bGhostDelayActive)
	{
		m_fGhostDelayTimer -= DeltaTime;
		if (m_fGhostDelayTimer > 0.0f)
			return;
		m_bGhostDelayActive = false;
	}

	// 보간 처리
	if (!FMath::IsNearlyEqual(m_fGhostPercent, m_fTargetPercent, m_fGhostTolerance))
	{
		m_fGhostPercent = FMath::FInterpTo(m_fGhostPercent, m_fTargetPercent, DeltaTime, m_fGhostInterpSpeed);

		if (m_pGhostBarID)
		{
			m_pGhostBarID->SetScalarParameterValue(PARAM_PERCENT, m_fGhostPercent);
		}
	}
	else
	{
		m_fGhostPercent = m_fTargetPercent;
		if (m_pGhostBarID)
		{
			m_pGhostBarID->SetScalarParameterValue(PARAM_PERCENT, m_fGhostPercent);
		}
		EnableTick(false);
	}
}

float UBarWidget::GetDeltaTime(float InDeltaTime) const
{
	if (m_bUseRealTime)
	{
		// 실제 컴퓨터 시간을 가져오는 함수(체인 어택 시 월드는 느려지되, 타이머는 실제 시간으로 돌아가기 때문)
		return FApp::GetDeltaTime();
	}
	return InDeltaTime;
}
