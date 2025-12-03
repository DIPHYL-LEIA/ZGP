// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadManagerComponent.generated.h"

class APawn;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API USquadManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquadManagerComponent();

	void InitActiveCharacter(APawn* Character);
	void RegisterCharacter(APawn* Character);
	void RequestTag();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<TObjectPtr<APawn>> m_arSquadCharacter;
	UPROPERTY()
	TObjectPtr<APawn> m_pActiveCharacter;

	void DoTag(APawn* InCharacter, APawn* OutCharacter);

private:
	void CalculateTagSpawnTransform(const APawn* BaseCharacter, FVector& OutLocation, FRotator& OutRotation) const;
	bool ValidTagSpawnLocation(const FVector& Location, FVector& ValidLocation) const;

	// 캐릭터 앞 나올 위치 지정
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	float m_fTagSpawnDistance = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	float m_fTagSpawnSideOffset = 1.f;
};
