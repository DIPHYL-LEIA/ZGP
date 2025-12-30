// Fill out your copyright notice in the Description page of Project Settings.


#include "ChainAttackComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UChainAttackComponent::UChainAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UChainAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UChainAttackComponent::TriggerChainAttack(AActor* TargetEnemy)
{
	if (m_eChainState != EChainAttackState::INACTIVE)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ChainAttackComponent] : Already Active"));
		return false;
	}

	if (!TargetEnemy) return false;

	m_pChainTarget = TargetEnemy;

	SetChainState(EChainAttackState::WAITING_INPUT);

	StartSlowMotion();
	StartCameraEffect();
	StartWindowTimer();

	OnChainAttackReady.Broadcast(TargetEnemy);

	return true;
}

bool UChainAttackComponent::ExecuteChainAttack()
{
	if (m_eChainState != EChainAttackState::WAITING_INPUT) return false;
	if (!m_pChainTarget.IsValid())
	{
		CancelChainAttack();
		return false;
	}

	ClearWindowTimer();

	SetChainState(EChainAttackState::EXECUTE);

	EndSlowMotion();
	EndCameraEffect();

	OnChainAttackExecuted.Broadcast();

	return true;
}

void UChainAttackComponent::CancelChainAttack()
{
	if (m_eChainState == EChainAttackState::INACTIVE) return;

	ClearWindowTimer();

	if (m_eChainState == EChainAttackState::WAITING_INPUT)
	{
		EndSlowMotion();
		EndCameraEffect();
	}

	m_pChainTarget.Reset();

	SetChainState(EChainAttackState::INACTIVE);

	OnChainAttackCancelled.Broadcast();
}

void UChainAttackComponent::FinishChainAttack()
{
	if (m_eChainState != EChainAttackState::EXECUTE) return;

	m_pChainTarget.Reset();

	SetChainState(EChainAttackState::INACTIVE);

	OnChainAttackFinished.Broadcast();
}

bool UChainAttackComponent::IsChainWaitingInput() const
{
	return m_eChainState == EChainAttackState::WAITING_INPUT;
}

bool UChainAttackComponent::IsChainExecuteing() const
{
	return m_eChainState == EChainAttackState::EXECUTE;
}

bool UChainAttackComponent::IsChainActive() const
{
	return m_eChainState != EChainAttackState::INACTIVE;
}

AActor* UChainAttackComponent::GetChainTarget() const
{
	return m_pChainTarget.Get();
}

void UChainAttackComponent::SetChainState(EChainAttackState NewState)
{
	if (m_eChainState == NewState) return;

	EChainAttackState OldState = m_eChainState;
	m_eChainState = NewState;

	OnChainAttackStateChanged.Broadcast(OldState, NewState);
}

void UChainAttackComponent::StartSlowMotion()
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 시간 느리게
	UGameplayStatics::SetGlobalTimeDilation(World, m_fGlobalTime);

	AActor* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn)
	{
		PlayerPawn->CustomTimeDilation = 1.0f / m_fGlobalTime;
	}

	if (m_pChainTarget.IsValid())
	{
		m_fOriginalTargetTime = m_pChainTarget->CustomTimeDilation;
		m_pChainTarget->CustomTimeDilation = m_fTargetTime;
	}
	UE_LOG(LogTemp, Log, TEXT("[ChainAttack] SlowMotion Activated - Global: %.2f"), m_fGlobalTime);
}

void UChainAttackComponent::EndSlowMotion()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);

	AActor* PlayerPawn = GetPlayerPawn();
	if (PlayerPawn)
	{
		PlayerPawn->CustomTimeDilation = 1.0f;
	}

	if (m_pChainTarget.IsValid())
	{
		m_pChainTarget->CustomTimeDilation = m_fOriginalTargetTime;
	}
}

void UChainAttackComponent::StartWindowTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ClearWindowTimer();

	float RealDuration = m_fInputTime * m_fGlobalTime;

	World->GetTimerManager().SetTimer(m_WindowTimerHandle, this, &UChainAttackComponent::HandleWindowTimeOut, RealDuration, false);
}

void UChainAttackComponent::ClearWindowTimer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (m_WindowTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(m_WindowTimerHandle);
	}
}

void UChainAttackComponent::HandleWindowTimeOut()
{
	CancelChainAttack();
}

void UChainAttackComponent::StartCameraEffect()
{
}

void UChainAttackComponent::EndCameraEffect()
{
}

AActor* UChainAttackComponent::GetPlayerPawn() const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return nullptr;

	return PC->GetPawn();
}
