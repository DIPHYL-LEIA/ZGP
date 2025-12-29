// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyDazeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "HealthComponent.h"
#include "ActionStateProvider.h"
#include "ActionState.h"

AEnemyCharacter::AEnemyCharacter()
{
	m_pEnemyDazeComponent = CreateDefaultSubobject<UEnemyDazeComponent>(TEXT("EnemyDazeComponent"));

	// Capsule Collision 
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.f);

	// Mesh
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	// Controller Rotation
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

}

UEnemyDazeComponent* AEnemyCharacter::GetDazeComponent() const
{
	return m_pEnemyDazeComponent;
}

bool AEnemyCharacter::IsTargetable_Implementation() const
{
	return !IsActionState(EActionState::DEAD);
}

FVector AEnemyCharacter::GetTargetLocation_Implementation() const
{
	return GetMesh()->GetSocketLocation(TEXT("Humanoid_-Spine2"));
}

void AEnemyCharacter::OnTargeted_Implementation(bool IsTargeted)
{
}

void AEnemyCharacter::OnUnTargeted_Implementation()
{
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

void AEnemyCharacter::ApplyCombatEffect_Implementation(const FDamageData& DamageData)
{
	// Daze 상태에서 강공격 받을 시 Chain Attck
	if (m_pEnemyDazeComponent && m_pEnemyDazeComponent->IsDazed())
	{
		if (DamageData.bCanChainAttack)
		{
			m_pEnemyDazeComponent->TriggerChainAttack();
		}
	}

	Super::ApplyCombatEffect_Implementation(DamageData);

	if (m_pEnemyDazeComponent && !m_pEnemyDazeComponent->IsDazed())
	{
		m_pEnemyDazeComponent->TakeDaze(DamageData.DazeValue);
	}
}

void AEnemyCharacter::HandleDaze()
{
	SetActionState(EActionState::DAZED);
}

void AEnemyCharacter::HandleDazeRecovered()
{
	UE_LOG(LogTemp, Warning, TEXT("[Enemy] HandleDazeRecovered CALLED - IsDead: %s"),
		IsActionState(EActionState::DEAD) ? TEXT("true") : TEXT("false"));

	if (IsActionState(EActionState::DEAD) == false)
	{
		bool bCanChange = CanChangeActionState(EActionState::IDLE);
		UE_LOG(LogTemp, Warning, TEXT("[Enemy] CanChangeActionState(IDLE): %s"),
			bCanChange ? TEXT("true") : TEXT("false"));

		SetActionState(EActionState::IDLE);

		UE_LOG(LogTemp, Warning, TEXT("[Enemy] After SetActionState - IsIdle: %s"),
			IsActionState(EActionState::IDLE) ? TEXT("true") : TEXT("false"));
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
