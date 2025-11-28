// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "StatsComponent.h"
#include "ActionStateComponent.h"
#include "SkillComponent.h"
#include "HealthComponent.h"
#include "AttributeAnomalyComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_pActionStateComponent = CreateDefaultSubobject<UActionStateComponent>(TEXT("ActionStateComponent"));
	m_pSkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	m_pHealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	m_pAttributeAnomalyComponent = CreateDefaultSubobject<UAttributeAnomalyComponent>(TEXT("AttributeAnomalyComponent"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (m_pSkillComponent)
	{
		m_pSkillComponent->OnRequestPlayMontage.AddDynamic(this, &ABaseCharacter::HandlePlayMontage);
	}
}


void ABaseCharacter::HandlePlayMontage(UAnimMontage* MontagePlay)
{
	if (MontagePlay && GetMesh() && GetMesh()->GetAnimInstance())
	{
		const float Duration = PlayAnimMontage(MontagePlay);

		if (Duration > 0.f)
		{
			UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
			AnimInst->OnMontageEnded.RemoveDynamic(this, &ABaseCharacter::HandleMontageEnded);
			AnimInst->OnMontageEnded.AddDynamic(this, &ABaseCharacter::HandleMontageEnded);
		}
	}
}

void ABaseCharacter::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &ABaseCharacter::HandleMontageEnded);
	}

	if (bInterrupted) return;

	if (m_pActionStateComponent && m_pActionStateComponent->IsTemporaryState())
	{
		if (IsActionState(EActionState::DEAD) == false)
		{
			SetActionState(EActionState::IDLE);
		}
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


bool ABaseCharacter::CanChangeActionState(EActionState NewState) const
{
	if (m_pActionStateComponent)
	{
		return m_pActionStateComponent->CanChangeActionState(NewState);
	}
	return false;
}

void ABaseCharacter::SetActionState(EActionState NewState)
{
	if (m_pActionStateComponent)
	{
		m_pActionStateComponent->SetActionState(NewState);
	}
}

bool ABaseCharacter::IsActionState(EActionState State) const
{
	if (m_pActionStateComponent)
	{
		return m_pActionStateComponent->IsActionState(State);
	}
	return false;
}

void ABaseCharacter::ApplyCombatEffect_Implementation(const FDamageData& DamageData)
{
	if (m_pHealthComponent)
	{
		m_pHealthComponent->TakeDamage(DamageData.BaseDamageValue, DamageData.Attacker.Get());
	}

	if (m_pAttributeAnomalyComponent && DamageData.AnomalyType != EAttributeType::NONE)
	{
		m_pAttributeAnomalyComponent->TakeAnomalyDamage(DamageData.AnomalyValue, DamageData.AnomalyType);
	}

	if (CanChangeActionState(EActionState::HIT))
	{
		SetActionState(EActionState::HIT);
	}
}

bool ABaseCharacter::CanParry_Implementation() const
{
	return false;
}

void ABaseCharacter::Parried_Implementation(AActor* Character)
{
}


UActionStateComponent* ABaseCharacter::GetActionStateComponent() const
{
	return m_pActionStateComponent;
}

UHealthComponent* ABaseCharacter::GetHealthComponent() const
{
	return m_pHealthComponent;
}

UAttributeAnomalyComponent* ABaseCharacter::GetAttributeAnomalyComponent() const
{
	return m_pAttributeAnomalyComponent;
}

