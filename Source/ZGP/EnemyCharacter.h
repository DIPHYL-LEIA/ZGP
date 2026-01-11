// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Targetable.h"
#include "Dazeable.h"
#include "Enemy/EnemyType.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ZGP_API AEnemyCharacter : public ABaseCharacter, public ITargetable, public IDazeable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyDazeComponent> m_pEnemyDazeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyUIComponent> m_pEnemyUIComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	EEnemyType m_eEnemyType = EEnemyType::NORMAL;

public:
	AEnemyCharacter();

	UFUNCTION(BlueprintPure)
	EEnemyType GetEnemyType() const { return m_eEnemyType; }

	UEnemyDazeComponent* GetDazeComponent() const;
	class UEnemyUIComponent* GetUIComponent() const;

	virtual bool IsTargetable_Implementation() const override;
	virtual FVector GetTargetLocation_Implementation() const override;
	virtual void OnTargeted_Implementation(bool IsTargeted) override;
	virtual void OnUnTargeted_Implementation() override;

	virtual void PauseDazeTimer_Implementation(bool bPause) override;
	virtual bool IsDazed_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;

	UFUNCTION()
	void BindUIComponent();

	UFUNCTION()
	void HandleDaze();

	UFUNCTION()
	void HandleDazeRecovered();

	UFUNCTION()
	void HandleHealthStateChanged(AActor* Character, float NewHealth, float MaxHealth);
	
	UFUNCTION()
	void HandleDazeValueChanged(float CurrentDaze, float MaxDaze);

	UFUNCTION()
	void HandleDied(AActor* Character);

};
