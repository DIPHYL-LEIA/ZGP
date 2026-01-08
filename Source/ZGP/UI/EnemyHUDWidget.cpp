// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHUDWidget.h"
#include "BarWidget.h"
#include "Components/TextBlock.h"
#include "../EnemyCharacter.h"
#include "../EnemyDazeComponent.h"
#include "../HealthComponent.h"

void UEnemyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyHUDWidget::NativeDestruct()
{
	ClearBinding();
	Super::NativeDestruct();
}

void UEnemyHUDWidget::BindEnemy(AEnemyCharacter* Enemy)
{
	if (m_pBoundEnemy.IsValid())
	{
		ClearBinding();
	}

	if (!Enemy)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	m_pBoundEnemy = Enemy;

	// Health Component 바인딩
	m_pHealthComponent = Enemy->FindComponentByClass<UHealthComponent>();
	if (m_pHealthComponent.IsValid())
	{
		m_pHealthComponent->OnHealthChanged.AddDynamic(this, &UEnemyHUDWidget::HandleHealthChanged);

		float Current = m_pHealthComponent->GetCurrentHealth();
		float Max = m_pHealthComponent->GetMaxHealth();
		if (HPBar && Max > 0.f)
		{
			HPBar->SetPercentNow(Current / Max);
		}
	}

	// Daze Component 바인딩
	m_pEnemyDazeComponent = Enemy->GetDazeComponent();
	if (m_pEnemyDazeComponent.IsValid())
	{
		m_pEnemyDazeComponent->OnDazeChanged.AddDynamic(this, &UEnemyHUDWidget::HandleDazeChanged);

		float Current = m_pEnemyDazeComponent->GetCurrentDaze();
		float Max = m_pEnemyDazeComponent->GetMaxDaze();
		if (DazeBar && DazeValue)
		{
			DazeBar->SetPercentNow(Max > 0.f ? (Current / Max) : 0.f);
			DazeValue->SetText(FText::AsNumber(FMath::FloorToInt(Current)));
		}
	}
}

void UEnemyHUDWidget::UnbindEnemy()
{
	ClearBinding();
	SetVisibility(ESlateVisibility::Collapsed);
}

AEnemyCharacter* UEnemyHUDWidget::GetBoundEnemy() const
{
	return m_pBoundEnemy.Get();
}

void UEnemyHUDWidget::HandleHealthChanged(AActor* Character, float CurrentHealth, float MaxHealth)
{
	if (HPBar && MaxHealth > 0.f)
	{
		HPBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

void UEnemyHUDWidget::HandleDazeChanged(float CurrentDaze, float MaxDaze)
{
	if (MaxDaze <= 0.f) return;
	if (DazeBar)
	{
		DazeBar->SetPercent(CurrentDaze / MaxDaze);
	}
	if (DazeValue)
	{
		DazeValue->SetText(FText::AsNumber(FMath::FloorToInt(CurrentDaze)));
	}
}

void UEnemyHUDWidget::ClearBinding()
{
	if (m_pHealthComponent.IsValid())
	{
		m_pHealthComponent->OnHealthChanged.RemoveDynamic(this, &UEnemyHUDWidget::HandleHealthChanged);
	}
	m_pHealthComponent.Reset();

	if (m_pEnemyDazeComponent.IsValid())
	{
		m_pEnemyDazeComponent->OnDazeChanged.RemoveDynamic(this, &UEnemyHUDWidget::HandleDazeChanged);
	}
	m_pEnemyDazeComponent.Reset();

	m_pBoundEnemy.Reset();
}
