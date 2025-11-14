// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyDazeComponent.h"
#include "TimerManager.h"

UEnemyDazeComponent::UEnemyDazeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	m_fCurrentDaze = 0.f;
	m_fMaxDaze = 100.f;
	m_fDazeDuration = 5.f;
	m_bIsDazed = false;
}

void UEnemyDazeComponent::InitializeComponent()
{
	Super::InitializeComponent();

	m_fCurrentDaze = 0.f;
	m_bIsDazed = false;
}

void UEnemyDazeComponent::TakeDaze(float Value)
{
	if (m_bIsDazed || Value <= 0.f) return;

	m_fCurrentDaze = FMath::Clamp(m_fCurrentDaze + Value, 0.f, m_fMaxDaze);

	OnDazeChanged.Broadcast(m_fCurrentDaze, m_fMaxDaze);
	UE_LOG(LogTemp, Log, TEXT("TakeDaze : %f / %f"), m_fCurrentDaze, m_fMaxDaze);

	if (m_fCurrentDaze >= m_fMaxDaze)
	{
		m_bIsDazed = true;

		OnDazed.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Daze State : DAZED"));

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DazeRecoverTimerHandle, this, &UEnemyDazeComponent::RecoverDaze, m_fDazeDuration, false);
		}
	}
}

void UEnemyDazeComponent::RecoverDaze()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DazeRecoverTimerHandle);
	}

	m_bIsDazed = false;
	m_fCurrentDaze = 0.f;

	OnDazeRecovered.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Daze State : RECOVERED"));

	// UI 초기화용 수치 변화 방송
	OnDazeChanged.Broadcast(m_fCurrentDaze, m_fMaxDaze);
}
