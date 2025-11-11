// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadManagerComponent.h"
#include "Taggable.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"		// GetOwner, Possess

USquadManagerComponent::USquadManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_pActiveCharacter = nullptr;
}

void USquadManagerComponent::InitActiveCharacter(APawn* Character)
{
	if (m_arSquadCharacter.Contains(Character))
	{
		m_pActiveCharacter = Character;
	}
}

void USquadManagerComponent::RegisterCharacter(APawn* Character)
{
	if (Character == nullptr) return;

	if (Character->Implements<UTaggable>())
	{
		m_arSquadCharacter.Add(Character);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManagerComp] : FAILED"));
	}
}

void USquadManagerComponent::RequestTag()
{
	if (m_arSquadCharacter.Num() < 2) return;
	if (m_pActiveCharacter == nullptr)
	{
		m_pActiveCharacter = m_arSquadCharacter[0];
	}

	APawn* StandbyCharacter = (m_arSquadCharacter[0] == m_pActiveCharacter) ? m_arSquadCharacter[1] : m_arSquadCharacter[0];

	if (m_pActiveCharacter == nullptr || StandbyCharacter == nullptr) return;

	// C++에서 인터페이스 호출할 때 'Execute_함수명'
	if (ITaggable::Execute_CanTagOut(m_pActiveCharacter))
	{
		DoTag(StandbyCharacter, m_pActiveCharacter);
		m_pActiveCharacter = StandbyCharacter;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManagerComp] : Tag request DENIED"));
	}

}

void USquadManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USquadManagerComponent::DoTag(APawn* InCharacter, APawn* OutCharacter)
{
	const FVector Location = OutCharacter->GetActorLocation();
	const FRotator Rotation = OutCharacter->GetActorRotation();

	ITaggable::Execute_ExecuteTagOut(OutCharacter);
	ITaggable::Execute_ExecuteTagIn(InCharacter, Location, Rotation);

	// Player Controller에 Possess
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		PC->Possess(InCharacter);
	}
}

