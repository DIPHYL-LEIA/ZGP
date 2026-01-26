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
	if (!World) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return;

	if (m_arSquadCharacterClass.Num() == 0) return;

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
		}
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

APawn* USquadManagerComponent::GetCharacterIndex(int32 Index) const
{
	if (m_arSquadCharacter.IsValidIndex(Index))
	{
		return m_arSquadCharacter[Index];
	}
	return nullptr;
}

TArray<int32> USquadManagerComponent::GetStandbyCharacterIndices() const
{
	TArray<int32> StandbyIndices;
	for (int32 i = 0; i < m_arSquadCharacter.Num(); ++i)
	{
		if (i != m_nActiveCharacterIndex)
		{
			StandbyIndices.Add(i);
		}
	}
	return StandbyIndices;
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

	int32 NextIndex = GetNextCharacterIndex();
	APawn* NextCharacter = GetCharacterIndex(NextIndex);

	if (!m_pActiveCharacter || !NextCharacter) return;

	if (ITaggable::Execute_CanTag(m_pActiveCharacter))
	{
		DoTag(NextCharacter, m_pActiveCharacter);
		m_nActiveCharacterIndex = NextIndex;
		m_pActiveCharacter = NextCharacter;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SquadManagerComp] : Tag request DENIED"));
	}

}

void USquadManagerComponent::RequestParryTag(AActor* ParriedEnemy)
{
	if (m_arSquadCharacter.Num() < 2) return;

	int32 NextIndex = GetNextCharacterIndex();
	APawn* NextCharacter = GetCharacterIndex(NextIndex);

	if (!m_pActiveCharacter || !NextCharacter) return;

	APawn* PrevCharacter = m_pActiveCharacter;

	DoParryTag(NextCharacter, PrevCharacter);
	m_nActiveCharacterIndex = NextIndex;
	m_pActiveCharacter = NextCharacter;

	OnParryTagExecute.Broadcast(NextCharacter, ParriedEnemy);

	UE_LOG(LogTemp, Log, TEXT("[SquadManagerComponent] : Parry Tag Success"));
}

void USquadManagerComponent::RequestChainAttack(int32 SelectedIndex, AActor* TargetEnemy)
{
	if (m_arSquadCharacter.Num() < 2) return;
	if (!TargetEnemy) return;
	if (!m_arSquadCharacter.IsValidIndex(SelectedIndex)) return;
	if (SelectedIndex == m_nActiveCharacterIndex) return;

	APawn* SelectedCharacter = GetCharacterIndex(SelectedIndex);
	if (!m_pActiveCharacter || !SelectedCharacter) return;

	APawn* PrevCharacter = m_pActiveCharacter;

	DoChainAttack(SelectedCharacter, PrevCharacter, TargetEnemy);
	m_nActiveCharacterIndex = SelectedIndex;
	m_pActiveCharacter = SelectedCharacter;

	OnChainAttackExecute.Broadcast(SelectedCharacter, TargetEnemy);

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

int32 USquadManagerComponent::GetNextCharacterIndex() const
{
	if (m_arSquadCharacter.Num() <= 1) return 0;
	return (m_nActiveCharacterIndex + 1) % m_arSquadCharacter.Num();
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
	UWorld* World = GetWorld();
	if (!World) return false;

	// 벽/장애물 충돌 검사
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(34.f, 88.f);

	if (World->OverlapBlockingTestByChannel(Location, FQuat::Identity, ECC_Pawn, CapsuleShape))
	{
		return false;
	}

	FHitResult FloorHit;
	FVector TraceStart = Location;
	FVector TraceEnd = Location - FVector(0.f, 0.f, 500.f);

	if (World->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		ValidLocation = Location;
		return true;
	}
	return false;
}

