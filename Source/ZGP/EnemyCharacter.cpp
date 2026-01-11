// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "EnemyDazeComponent.h"
#include "Enemy/EnemyUIComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "HealthComponent.h"
#include "ActionStateProvider.h"
#include "ActionState.h"
#include "SquadAction.h"

AEnemyCharacter::AEnemyCharacter()
{
	m_pEnemyDazeComponent = CreateDefaultSubobject<UEnemyDazeComponent>(TEXT("EnemyDazeComponent"));

	// Capsule Collision 
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	// Mesh
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Controller Rotation
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.f, 500.f, 0.f);
	}


}

UEnemyDazeComponent* AEnemyCharacter::GetDazeComponent() const
{
	return m_pEnemyDazeComponent;
}

UEnemyUIComponent* AEnemyCharacter::GetUIComponent() const
{
	return nullptr;
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
	if (m_pEnemyUIComponent)
	{
		m_pEnemyUIComponent->OnTargetStateChanged(true, IsTargeted);
	}
}

void AEnemyCharacter::OnUnTargeted_Implementation()
{
	if (m_pEnemyUIComponent)
	{
		m_pEnemyUIComponent->OnTargetStateChanged(false, false);
	}
}

void AEnemyCharacter::PauseDazeTimer_Implementation(bool bPause)
{
	if (m_pEnemyDazeComponent)
	{
		m_pEnemyDazeComponent->PauseDazeTimer(bPause);
	}
}

bool AEnemyCharacter::IsDazed_Implementation() const
{
	if (m_pEnemyDazeComponent)
	{
		return m_pEnemyDazeComponent->IsDazed();
	}
	return false;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindUIComponent();

	if (m_pEnemyDazeComponent)
	{
		m_pEnemyDazeComponent->OnDazed.AddDynamic(this, &AEnemyCharacter::HandleDaze);
		m_pEnemyDazeComponent->OnDazeRecovered.AddDynamic(this, &AEnemyCharacter::HandleDazeRecovered);
	}

	if (m_pEnemyUIComponent)
	{
		float HealthPercent = 1.0f;
		float DazePercent = 0.0f;

		if (m_pHealthComponent && m_pHealthComponent->GetMaxHealth() > 0.f)
		{
			HealthPercent = m_pHealthComponent->GetCurrentHealth() / m_pHealthComponent->GetMaxHealth();
		}

		if (m_pEnemyDazeComponent && m_pEnemyDazeComponent->GetMaxDaze() > 0.f)
		{
			DazePercent = m_pEnemyDazeComponent->GetCurrentDaze() / m_pEnemyDazeComponent->GetMaxDaze();
		}

		m_pEnemyUIComponent->InitializeValue(HealthPercent, DazePercent);
	}
}

void AEnemyCharacter::ApplyCombatEffect_Implementation(const FDamageData& DamageData)
{
	// Daze 상태에서 강공격 받을 시 Chain Attck
	if (m_pEnemyDazeComponent && m_pEnemyDazeComponent->IsDazed())
	{
		if (DamageData.bCanChainAttack)
		{
			if (DamageData.Attacker.IsValid())
			{
				APawn* Attacker = Cast<APawn>(DamageData.Attacker.Get());
				if (Attacker)
				{
					AController* AttackerController = Attacker->GetController();
					if (AttackerController && AttackerController->Implements<USquadAction>())
					{
						ISquadAction::Execute_TryTriggerChainAttack(AttackerController, this);
					}
				}
			}
			m_pEnemyDazeComponent->TriggerChainAttack();
		}
	}

	Super::ApplyCombatEffect_Implementation(DamageData);

	if (m_pEnemyDazeComponent && !m_pEnemyDazeComponent->IsDazed())
	{
		m_pEnemyDazeComponent->TakeDaze(DamageData.DazeValue);
	}
}

void AEnemyCharacter::BindUIComponent()
{
	if (m_pHealthComponent)
	{
		m_pHealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyCharacter::HandleHealthStateChanged);
		m_pHealthComponent->OnDied.AddDynamic(this, &AEnemyCharacter::HandleDied);
	}

	// Daze → EnemyUIComponent 중개
	if (m_pEnemyDazeComponent)
	{
		m_pEnemyDazeComponent->OnDazeChanged.AddDynamic(this, &AEnemyCharacter::HandleDazeValueChanged);
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
	if (m_pEnemyUIComponent)
	{
		m_pEnemyUIComponent->OnHealthChanged(Character, NewHealth, MaxHealth);
	}
}

void AEnemyCharacter::HandleDazeValueChanged(float CurrentDaze, float MaxDaze)
{
	if (m_pEnemyUIComponent)
	{
		m_pEnemyUIComponent->OnDazeChanged(CurrentDaze, MaxDaze);
	}
}

void AEnemyCharacter::HandleDied(AActor* Character)
{
	SetActionState(EActionState::DEAD);

	// 사망 처리
}
