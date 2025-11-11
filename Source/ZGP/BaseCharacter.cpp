// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "StatsComponent.h"
#include "ActionStateComponent.h"
#include "SkillComponent.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_pActionStateComp = CreateDefaultSubobject<UActionStateComponent>(TEXT("ActionStateComponent"));
	m_pSkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
}

bool ABaseCharacter::CanChangeActionState(EActionState NewState) const
{
	if (m_pActionStateComp)
	{
		return m_pActionStateComp->CanChangeActionState(NewState);
	}
	return false;
}

void ABaseCharacter::SetActionState(EActionState NewState) const
{
	if (m_pActionStateComp)
	{
		m_pActionStateComp->SetActionState(NewState);
	}
}

bool ABaseCharacter::IsActionState(EActionState State) const
{
	if (m_pActionStateComp)
	{
		return m_pActionStateComp->IsActionState(State);
	}
	return false;
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
	if (MontagePlay)
	{
		PlayAnimMontage(MontagePlay);
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

UActionStateComponent* ABaseCharacter::GetActionStateComponent() const
{
	return m_pActionStateComp;
}

