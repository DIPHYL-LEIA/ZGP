// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Targetable.h"
#include "EnemyCharacter.generated.h"

/**
 *
 */
UCLASS()
class ZGP_API AEnemyCharacter : public ABaseCharacter, public ITargetable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyDazeComponent> m_pEnemyDazeComponent;

public:
	AEnemyCharacter();

	UEnemyDazeComponent* GetDazeComponent() const;

	virtual bool IsTargetable_Implementation() const override;
	virtual FVector GetTargetLocation_Implementation() const override;
	virtual void OnTargeted_Implementation(bool IsTargeted) override;
	virtual void OnUnTargeted_Implementation() override;

protected:
	virtual void BeginPlay() override;
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;

	UFUNCTION()
	void HandleDaze();

	UFUNCTION()
	void HandleDazeRecovered();

	UFUNCTION()
	void HandleHealthStateChanged(AActor* Character, float NewHealth, float MaxHealth);

	UFUNCTION()
	void HandleDied(AActor* Character);

};
