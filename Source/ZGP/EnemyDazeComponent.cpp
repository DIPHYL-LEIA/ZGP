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
    m_bDazeTimerPause = false;
}

void UEnemyDazeComponent::InitializeComponent()
{
	Super::InitializeComponent();

	m_fCurrentDaze = 0.f;
	m_bIsDazed = false;
    m_bDazeTimerPause = false;
}

void UEnemyDazeComponent::TakeDaze(float Value)
{
    if (m_bIsDazed || Value <= 0.f) return;

    m_fCurrentDaze = FMath::Clamp(m_fCurrentDaze + Value, 0.f, m_fMaxDaze);

    OnDazeChanged.Broadcast(m_fCurrentDaze, m_fMaxDaze);

    if (m_fCurrentDaze >= m_fMaxDaze)
    {
        m_bIsDazed = true;
        OnDazed.Broadcast();

        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().SetTimer(
                DazeRecoverTimerHandle,
                this,
                &UEnemyDazeComponent::RecoverDaze,
                m_fDazeDuration,
                false);
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
    m_bDazeTimerPause = false;
    m_fCurrentDaze = 0.f;

    OnDazeRecovered.Broadcast();
    OnDazeChanged.Broadcast(m_fCurrentDaze, m_fMaxDaze);
}

void UEnemyDazeComponent::PauseDazeTimer(bool bPause)
{
    UWorld* World = GetWorld();
    if (!World) return;

    if (bPause && !m_bDazeTimerPause)
    {
        World->GetTimerManager().PauseTimer(DazeRecoverTimerHandle);
        m_bDazeTimerPause = true;
        UE_LOG(LogTemp, Log, TEXT("[DazeComp] Daze Timer PAUSED"));
    }
    else if (!bPause && m_bDazeTimerPause)
    {
        World->GetTimerManager().UnPauseTimer(DazeRecoverTimerHandle);
        m_bDazeTimerPause = false;
        UE_LOG(LogTemp, Log, TEXT("[DazeComp] Daze Timer RESUMED"));
    }
}

void UEnemyDazeComponent::TriggerChainAttack()
{
    if (!m_bIsDazed) return;
    UE_LOG(LogTemp, Warning, TEXT("[DazeComp] Chain Attack Triggered!"));
    OnChainAttack.Broadcast(GetOwner());
}
