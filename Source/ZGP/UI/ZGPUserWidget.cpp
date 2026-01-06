// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPUserWidget.h"
#include "Animation/WidgetAnimation.h"

UZGPUserWidget::UZGPUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UZGPUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	m_ShowAnimFinishedDelegate.BindDynamic(this, &UZGPUserWidget::HandleShowAnimationFinished);
	m_HideAnimFinishedDelegate.BindDynamic(this, &UZGPUserWidget::HandleHideAnimationFinished);

	if (m_bStartHidden)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		m_bIsVisible = false;
	}
	else
	{
		m_bIsVisible = (GetVisibility() != ESlateVisibility::Collapsed && GetVisibility() != ESlateVisibility::Hidden);
	}

	SetWidgetTickEnabled(false);
}

void UZGPUserWidget::NativeDestruct()
{
	if (ShowAnimation)
		UnbindAllFromAnimationFinished(ShowAnimation);
	if (HideAnimation)
		UnbindAllFromAnimationFinished(HideAnimation);

	m_ShowAnimFinishedDelegate.Unbind();
	m_HideAnimFinishedDelegate.Unbind();

	Super::NativeDestruct();
}

void UZGPUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (m_bTickEnabled && m_bIsVisible)
	{
		UpdateWidget(InDeltaTime);
	}
}

void UZGPUserWidget::ShowWidget()
{
	if (m_bIsVisible) return;

	m_bIsVisible = true;
	SetVisibility(m_eShowVisibility);

	if (ShowAnimation)
	{
		// 다른 애니메이션 델리게이트 해제
		if (HideAnimation)
			UnbindAllFromAnimationFinished(HideAnimation);

		// 중복 방지
		UnbindAllFromAnimationFinished(ShowAnimation);
		BindToAnimationFinished(ShowAnimation, m_ShowAnimFinishedDelegate);

		PlayAnimation(ShowAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, m_fAnimationSpeed);
	}
	else
	{
		// 애니메이션 없으면 완료 처리
		HandleShowAnimationFinished();
	}
	OnShowWidget();
}

void UZGPUserWidget::HideWidget()
{
	if (!m_bIsVisible) return;

	if (HideAnimation)
	{
		if (ShowAnimation && IsAnimationPlaying(ShowAnimation))
		{
			StopAnimation(ShowAnimation);
		}

		UnbindAllFromAnimationFinished(HideAnimation);
		PlayAnimation(HideAnimation, 0.f, 1, EUMGSequencePlayMode::Forward, m_fAnimationSpeed);
		BindToAnimationFinished(HideAnimation, m_HideAnimFinishedDelegate);
	}
	else
	{
		HandleHideAnimationFinished();
	}
	OnHideWidget();
}

float UZGPUserWidget::InterpGaugeValue(float Current, float Target, float DeltaTime, float Speed)
{
	return FMath::FInterpTo(Current, Target, DeltaTime, Speed);
}

float UZGPUserWidget::InterpGaugeDirection(float Current, float Target, float DeltaTime, float DecreaseSpeed, float IncreaseSpeed)
{
	const float Speed = (Target < Current) ? DecreaseSpeed : IncreaseSpeed;
	return FMath::FInterpTo(Current, Target, DeltaTime, Speed);
}

E_UI_GAUGEBAR UZGPUserWidget::GetGaugeBar(float Current, float Target, float Tolerance) const
{
	const float Different = Target - Current;

	if (FMath::Abs(Different) <= Tolerance)
	{
		return E_UI_GAUGEBAR::IDLE;
	}

	return (Different > 0.f) ? E_UI_GAUGEBAR::INCREASE : E_UI_GAUGEBAR::DECREASE;
}

bool UZGPUserWidget::UpdateGhostBar(UPARAM(ref) float& OutCurrent, UPARAM(ref) float& OutGhost, float Target, float DeltaTime, E_UI_GAUGEBAR& OutBar)
{
	// Current는 빠르게 Target으로
	OutCurrent = FMath::FInterpTo(OutCurrent, Target, DeltaTime, m_fGhostBarIncreaseSpeed);

	// Ghost는 Current를 느리게 따라감
	OutGhost = InterpGaugeDirection(OutGhost, OutCurrent, DeltaTime, m_fGhostBarDecreaseSpeed, m_fGhostBarIncreaseSpeed);

	OutBar = GetGaugeBar(OutGhost, OutCurrent, m_fGaugeTolerance);

	// 아직 애니메이션 중인지 반환
	return (OutBar != E_UI_GAUGEBAR::IDLE);
}

void UZGPUserWidget::SetWidgetTickEnabled(bool bEnable)
{
	if (m_bTickEnabled == bEnable) return;

	m_bTickEnabled = bEnable;
}

void UZGPUserWidget::HandleShowAnimationFinished()
{
}

void UZGPUserWidget::HandleHideAnimationFinished()
{
	m_bIsVisible = false;
	SetVisibility(ESlateVisibility::Collapsed);
	SetWidgetTickEnabled(false);
}
