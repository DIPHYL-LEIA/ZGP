// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBarWidget.h"
#include "BarWidget.h"
#include "Components/TextBlock.h"

void UEnemyBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (HPBar)
	{
		HPBar->SetUseGhostBar(true);
	}

	if (DazeBar)
	{
		DazeBar->SetUseGhostBar(false);
	}
}

void UEnemyBarWidget::UpdateHealth(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercent(Percent);
	}
}

void UEnemyBarWidget::UpdateDaze(float Percent)
{
	if (DazeBar)
	{
		DazeBar->SetPercent(Percent);
	}
}

void UEnemyBarWidget::SetHealthNow(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercentNow(Percent);
	}
}

void UEnemyBarWidget::SetDazeNow(float Percent)
{
	if (DazeBar)
	{
		DazeBar->SetPercentNow(Percent);
	}
}

void UEnemyBarWidget::UpdateDazeValue(float CurrentDaze, float MaxDaze)
{
	if (DazeValue)
	{
		int32 DisplayValue = FMath::FloorToInt(CurrentDaze);
		DazeValue->SetText(FText::AsNumber(DisplayValue));
	}
}
