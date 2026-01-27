// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SquadManagerComponent.generated.h"

class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnParryTagExecute, APawn*, NewActiveCharacter, AActor*, ParriedEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChainAttackExecute, APawn*, NewActiveCharacter, AActor*, TargetEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSquadInitialized, APawn*, FirstCharacter);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API USquadManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquadManagerComponent();

	UFUNCTION()
	void InitializeSquad();

	UFUNCTION()
	APawn* GetFirstCharacter() const;

	UFUNCTION()
	APawn* GetActiveCharacter() const { return m_pActiveCharacter; }

	UFUNCTION(BlueprintPure, Category = "Squad")
	int32 GetActiveCharacterIndex() const { return m_nActiveCharacterIndex; };

	UFUNCTION(BlueprintPure, Category = "Squad")
	int32 GetSquadCount() const { return m_arSquadCharacter.Num(); }

	UFUNCTION(BlueprintPure, Category = "Squad")
	APawn* GetCharacterIndex(int32 Index) const;

	// Chain Attack UI
	UFUNCTION(BlueprintPure, Category = "Squad")
	TArray<int32> GetStandbyCharacterIndices() const;

	void RegisterCharacter(APawn* Character);

	UFUNCTION(BlueprintCallable, Category = "Tag")
	void RequestTag();

	UFUNCTION(BlueprintCallable, Category = "Tag")
	void RequestParryTag(AActor* ParriedEnemy);

	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	void RequestChainAttack(int32 SelectedIndex, AActor* TargetEnemy);

	UPROPERTY(BlueprintAssignable, Category = "Tag")
	FOnParryTagExecute OnParryTagExecute;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackExecute OnChainAttackExecute;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnSquadInitialized OnSquadInitialized;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Squad")
	TArray<TSubclassOf<APawn>> m_arSquadCharacterClass;
	UPROPERTY()
	TArray<TObjectPtr<APawn>> m_arSquadCharacter;
	UPROPERTY()
	TObjectPtr<APawn> m_pActiveCharacter;

	UPROPERTY(VisibleInstanceOnly, Category = "Squad")
	int32 m_nActiveCharacterIndex = 0;

	void DoTag(APawn* InCharacter, APawn* OutCharacter);
	void DoParryTag(APawn* InCharacter, APawn* OutCharacter);
	void DoChainAttack(APawn* InCharacter, APawn* OutCharacter, AActor* TargetEnemy);

private:
	int32 GetNextCharacterIndex() const;

	void CalculateTagSpawnTransform(const APawn* BaseCharacter, FVector& OutLocation, FRotator& OutRotation) const;
	void CalculateChainAttackSpawnTransform(AActor* TargetEnemy, FVector& OutLocation, FRotator& OutRotation) const;
	bool ValidTagSpawnLocation(const FVector& Location, FVector& ValidLocation) const;

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	float m_fTagSpawnDistance = 1.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	float m_fTagSpawnSideOffset = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	float m_fChainAttackSpawnDistance = 300.f;
};
