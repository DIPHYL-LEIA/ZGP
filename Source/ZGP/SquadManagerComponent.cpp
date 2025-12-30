// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadManagerComponent.h"
#include "Taggable.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"		// GetOwner, Possess
#include "GameFramework/GameModeBase.h"

USquadManagerComponent::USquadManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_pActiveCharacter = nullptr;
}

void USquadManagerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USquadManagerComponent::InitializeSquad()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManager] World is NULL"));
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManager] Owner is not PlayerController"));
		return;
	}

	if (m_arSquadCharacterClass.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManager] No character classes assigned!"));
		return;
	}

	// 스폰 위치
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	AGameModeBase* GameMode = World->GetAuthGameMode();
	if (GameMode)
	{
		AActor* PlayerStart = GameMode->FindPlayerStart(PC);
		if (PlayerStart)
		{
			SpawnLocation = PlayerStart->GetActorLocation();
			SpawnRotation = PlayerStart->GetActorRotation();
			UE_LOG(LogTemp, Log, TEXT("[SquadManager] PlayerStart found at: %s"), *SpawnLocation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SquadManager] PlayerStart NOT found! Using (0,0,0)"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SquadManager] GameMode is NULL!"));
	}

	// 기존 Pawn 정리
	if (PC->GetPawn())
	{
		PC->GetPawn()->Destroy();
	}

	// 캐릭터 스폰
	for (int32 i = 0; i < m_arSquadCharacterClass.Num(); ++i)
	{
		TSubclassOf<APawn> CharacterClass = m_arSquadCharacterClass[i];
		if (!CharacterClass) continue;

		APawn* NewCharacter = World->SpawnActor<APawn>(CharacterClass, SpawnLocation, SpawnRotation);
		if (!NewCharacter)
		{
			UE_LOG(LogTemp, Error, TEXT("[SquadManager] Failed to spawn character index %d"), i);
			continue;
		}

		RegisterCharacter(NewCharacter);

		if (i == 0)
		{
			m_pActiveCharacter = NewCharacter;
		}
		else
		{
			if (NewCharacter->Implements<UTaggable>())
			{
				ITaggable::Execute_OnTagOut(NewCharacter);
			}
			NewCharacter->SetActorHiddenInGame(true);
			NewCharacter->SetActorEnableCollision(false);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[SquadManager] Squad Initialized with %d characters"), m_arSquadCharacter.Num());
	OnSquadInitialized.Broadcast(m_pActiveCharacter);
}

APawn* USquadManagerComponent::GetFirstCharacter() const
{
	if (m_arSquadCharacter.Num() > 0)
	{
		return m_arSquadCharacter[0];
	}

	return nullptr;
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

void USquadManagerComponent::RequestChainAttack(AActor* TargetEnemy)
{
	if (m_arSquadCharacter.Num() < 2) return;
	if (!TargetEnemy) return;

	if (m_pActiveCharacter == nullptr)
	{
		m_pActiveCharacter = m_arSquadCharacter[0];
	}

	APawn* StandbyCharacter = (m_arSquadCharacter[0] == m_pActiveCharacter) ? m_arSquadCharacter[1] : m_arSquadCharacter[0];

	if (!m_pActiveCharacter || !StandbyCharacter) return;

	APawn* PreviousCharacter = m_pActiveCharacter;

	DoChainAttack(StandbyCharacter, PreviousCharacter, TargetEnemy);
	m_pActiveCharacter = StandbyCharacter;

	OnChainAttackExecute.Broadcast(StandbyCharacter, TargetEnemy);

	UE_LOG(LogTemp, Warning, TEXT("[SquadManagerComponent] : Chain Attack Tag Success"));
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

void USquadManagerComponent::DoChainAttack(APawn* InCharacter, APawn* OutCharacter, AActor* TargetEnemy)
{
	FVector TargetLocation;
	FRotator TargetRotation;

	CalculateChainAttackSpawnTransform(TargetEnemy, TargetLocation, TargetRotation);

	ITaggable::Execute_OnTagOutAction(OutCharacter);
	ITaggable::Execute_OnChainAttackTag(InCharacter, TargetLocation, TargetRotation, TargetEnemy);

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

void USquadManagerComponent::CalculateChainAttackSpawnTransform(AActor* TargetEnemy, FVector& OutLocation, FRotator& OutRotation) const
{
	if (!TargetEnemy) return;

	const FVector EnemyLocation = TargetEnemy->GetActorLocation();
	const FVector EnemyForward = TargetEnemy->GetActorForwardVector();

	// 적을 마주하는 앞에 스폰
	OutLocation = EnemyLocation + (EnemyForward * m_fChainAttackSpawnDistance);

	FVector DirectionEnemy = EnemyLocation - OutLocation;
	DirectionEnemy.Z = 0.f;
	if (!DirectionEnemy.IsNearlyZero())
	{
		OutRotation = DirectionEnemy.Rotation();
	}
	else
	{
		OutRotation = (-EnemyForward).Rotation();
	}
}

bool USquadManagerComponent::ValidTagSpawnLocation(const FVector& Location, FVector& ValidLocation) const
{
	// 충돌 검사 필요
	ValidLocation = Location;
	return true;
}

