// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyDazeComponent.h"
#include "HealthComponent.h"
#include "ActionStateProvider.h"
#include "ActionState.h"

AEnemyCharacter::AEnemyCharacter()
{
	m_pEnemyDazeComponent = CreateDefaultSubobject<UEnemyDazeComponent>(TEXT("EnemyDazeComponent"));
}

UEnemyDazeComponent* AEnemyCharacter::GetDazeComponent() const
{
	return m_pEnemyDazeComponent;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Binding Delegate
	if (m_pHealthComponent)
	{
		m_pHealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::HandleHealthStateChanged);
		m_pHealthComponent->OnDied.AddDynamic(this, &AEnemyCharacter::HandleDied);
	}

	if (m_pEnemyDazeComponent)
	{
		m_pEnemyDazeComponent->OnDazed.AddDynamic(this, &AEnemyCharacter::HandleDaze);
		m_pEnemyDazeComponent->OnDazeRecovered.AddDynamic(this, &AEnemyCharacter::HandleDazeRecovered);
	}
}

void AEnemyCharacter::ApplyDamage_Implementation(const FDamageData& DamageData)
{
	// 1. 체력
	if (m_pHealthComponent)
	{
		AActor* Character = DamageData.Attacker.Get();
		m_pHealthComponent->TakeDamage(DamageData.BaseDamageValue, Character);
	}

	// 2. 속성 이상
	if (m_pAttributeAnomalyComponent)
	{
	}

	// 3. Daze
	if (m_pEnemyDazeComponent)
	{
	}
}

void AEnemyCharacter::HandleDaze()
{
	SetActionState(EActionState::DAZED);
}

void AEnemyCharacter::HandleDazeRecovered()
{
	if (IsActionState(EActionState::DEAD) == false)
	{
		SetActionState(EActionState::IDLE);
	}
}


void AEnemyCharacter::HandleHealthStateChanged(AActor* Character, float NewHealth, float MaxHealth)
{
}

void AEnemyCharacter::HandleDied(AActor* Character)
{
	SetActionState(EActionState::DEAD);

	// 사망 처리
}
