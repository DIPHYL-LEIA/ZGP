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

	if (ITaggable::Execute_CanTag(m_pActiveCharacter))
	{
		DoTag(StandbyCharacter, m_pActiveCharacter);
		m_pActiveCharacter = StandbyCharacter;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManagerComp] : Tag request DENIED"));
	}

}

void USquadManagerComponent::RequestParryTag(AActor* ParriedEnemy)
{
	if (m_arSquadCharacter.Num() < 2) return;

	if (m_pActiveCharacter == nullptr)
	{
		m_pActiveCharacter = m_arSquadCharacter[0];
	}

	APawn* StandbyCharacter = (m_arSquadCharacter[0] == m_pActiveCharacter) ? m_arSquadCharacter[1] : m_arSquadCharacter[0];
	if (m_pActiveCharacter == nullptr || StandbyCharacter == nullptr) return;

	APawn* PreviousCharacter = m_pActiveCharacter;

	DoParryTag(StandbyCharacter, PreviousCharacter);
	m_pActiveCharacter = StandbyCharacter;

	OnParryTagExecute.Broadcast(StandbyCharacter, ParriedEnemy);

	UE_LOG(LogTemp, Log, TEXT("[SquadManagerComponent] : Parry Tag Success"));

}

void USquadManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USquadManagerComponent::DoTag(APawn* InCharacter, APawn* OutCharacter)
{
	FVector TargetLocation;
	FRotator TargetRotation;

	CalculateTagSpawnTransform(OutCharacter, TargetLocation, TargetRotation);

	ITaggable::Execute_OnTagOut(OutCharacter);
	ITaggable::Execute_OnTagIn(InCharacter, TargetLocation, TargetRotation);

	// Player Controller에 Possess
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		const FRotator CameraRotation = PC->GetControlRotation();

		PC->Possess(InCharacter);
		PC->SetControlRotation(CameraRotation);

		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->SetGameCameraCutThisFrame();
		}
	}
}

void USquadManagerComponent::DoParryTag(APawn* InCharacter, APawn* OutCharacter)
{
	FVector TargetLocation;
	FRotator TargetRotation;

	CalculateTagSpawnTransform(OutCharacter, TargetLocation, TargetRotation);

	ITaggable::Execute_OnTagOutAction(OutCharacter);
	ITaggable::Execute_OnTagIn(InCharacter, TargetLocation, TargetRotation);

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		const FRotator CameraRotation = PC->GetControlRotation();

		PC->Possess(InCharacter);
		PC->SetControlRotation(CameraRotation);

		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->SetGameCameraCutThisFrame();
		}
	}
}

void USquadManagerComponent::CalculateTagSpawnTransform(const APawn* BaseCharacter, FVector& OutLocation, FRotator& OutRotation) const
{
	if (!BaseCharacter) return;

	const FVector BaseLocation = BaseCharacter->GetActorLocation();
	const FRotator BaseRotation = BaseCharacter->GetActorRotation();

	// 로컬 좌표 -> 월드 좌표계 변환
	const FVector RightVector = FRotationMatrix(BaseRotation).GetScaledAxis(EAxis::Y);
	const FVector ForwardVector = FRotationMatrix(BaseRotation).GetScaledAxis(EAxis::X);

	// 오프셋
	FVector Offset = (RightVector * m_fTagSpawnSideOffset) + (ForwardVector * (-m_fTagSpawnDistance));

	// 최종 목표 위치
	FVector TargetLocation = BaseLocation + Offset;

	// 벽 뚫기 방지
	FVector ValidLocation;
	if (ValidTagSpawnLocation(TargetLocation, ValidLocation))
	{
		OutLocation = ValidLocation;
	}
	else
	{
		// 위치가 벽이면 원래 캐릭터 위치 사용
		OutLocation = BaseLocation;
	}

	// 회전은 현재 캐릭터와 동일
	OutRotation = BaseRotation;

}

bool USquadManagerComponent::ValidTagSpawnLocation(const FVector& Location, FVector& ValidLocation) const
{
	// 충돌 검사 필요
	ValidLocation = Location;
	return true;
}

