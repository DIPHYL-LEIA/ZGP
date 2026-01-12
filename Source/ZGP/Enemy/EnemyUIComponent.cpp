// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyUIComponent.h"
#include "Components/WidgetComponent.h"
#include "../UI/EnemyBarWidget.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UEnemyUIComponent::UEnemyUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UEnemyUIComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeBarWidget();

	if (m_bStartHidden)
	{
		SetUIVisibility(false);
	}

}

void UEnemyUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearHideTimer();

	if (m_pBarWidgetComponent)
	{
		m_pBarWidgetComponent->DestroyComponent();
		m_pBarWidgetComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UEnemyUIComponent::InitializeBarWidget()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;
	if (!m_BarWidgetClass) return;

	m_pBarWidgetComponent = NewObject<UWidgetComponent>(Owner, TEXT("BarWidgetComponent"));
	if (!m_pBarWidgetComponent) return;

	m_pBarWidgetComponent->RegisterComponent();
	m_pBarWidgetComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	m_pBarWidgetComponent->SetRelativeLocation(m_vBarOffset);

	m_pBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	m_pBarWidgetComponent->SetDrawAtDesiredSize(true);
	m_pBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	m_pBarWidgetComponent->SetWidgetClass(m_BarWidgetClass);

	// 위젯 인스턴스 생성
	if (UUserWidget* Widget = m_pBarWidgetComponent->GetWidget())
	{
		m_pBarWidget = Cast<UEnemyBarWidget>(Widget);
	}
}

void UEnemyUIComponent::InitializeValue(float HealthPercent, float DazePercent)
{
	if (m_pBarWidget.IsValid())
	{
		m_pBarWidget->SetHealthNow(HealthPercent);
		m_pBarWidget->SetDazeNow(DazePercent);

		m_pBarWidget->UpdateDazeValue(0.f, 100.f);
	}
}

void UEnemyUIComponent::OnHealthChanged(AActor* Character, float CurrentHealth, float MaxHealth)
{
	if (m_bIsDead) return;

	SetUIVisibility(true);

	if (!m_bIsHardLock)
	{
		StartHideTimer();
	}

	if (m_pBarWidget.IsValid() && MaxHealth > 0.f)
	{
		m_pBarWidget->UpdateHealth(CurrentHealth / MaxHealth);
	}
}

void UEnemyUIComponent::OnDazeChanged(float CurrentDaze, float MaxDaze)
{
	if (m_bIsDead) return;

	SetUIVisibility(true);

	if (!m_bIsHardLock)
	{
		StartHideTimer();
	}

	if (m_pBarWidget.IsValid() && MaxDaze > 0.f)
	{
		if (MaxDaze > 0.f)
		{
			m_pBarWidget->UpdateDazeValue(CurrentDaze, MaxDaze);
		}

		m_pBarWidget->UpdateDaze(CurrentDaze / MaxDaze);
	}
}

void UEnemyUIComponent::OnDied(AActor* Character)
{
	m_bIsDead = true;
	ClearHideTimer();
	SetUIVisibility(false);
}

void UEnemyUIComponent::OnTargetStateChanged(bool bIsTargeted, bool bIsHardLock)
{
	m_bIsHardLock = bIsTargeted && bIsHardLock;

	if (m_bIsDead) return;

	if (m_bIsHardLock)
	{
		ClearHideTimer();
		SetUIVisibility(true);
	}
	else if (bIsTargeted)
	{
		SetUIVisibility(true);
		StartHideTimer();
	}
	else
	{
		if (!m_UIHideTimerHandle.IsValid())
		{
			StartHideTimer();
		}
	}
}

void UEnemyUIComponent::ShowEnemyUI(bool bShow)
{
	if (m_bIsDead) return;

	if (bShow)
	{
		SetUIVisibility(true);
		if (!m_bIsHardLock)
		{
			StartHideTimer();
		}
	}
	else
	{
		SetUIVisibility(false);
		ClearHideTimer();
	}
}

void UEnemyUIComponent::SetUIVisibility(bool bVisible)
{
	if (m_bIsUIVisible == bVisible) return;

	m_bIsUIVisible = bVisible;

	if (m_pBarWidgetComponent)
	{
		m_pBarWidgetComponent->SetVisibility(bVisible);
	}
}

void UEnemyUIComponent::StartHideTimer()
{
	ClearHideTimer();

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(
		m_UIHideTimerHandle,
		this,
		&UEnemyUIComponent::HandleHideTimerExpired,
		m_fUIVisibleDuration,
		false
	);
}

void UEnemyUIComponent::ClearHideTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (m_UIHideTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(m_UIHideTimerHandle);
	}
}

void UEnemyUIComponent::HandleHideTimerExpired()
{
	if (m_bIsHardLock) return;

	SetUIVisibility(false);
}


