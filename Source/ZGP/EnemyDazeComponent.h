// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyDazeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDazeChanged, float, NewDaze, float, MaxDaze);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDazed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDazeRecovered);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChainAttack, AActor*, Character);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UEnemyDazeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyDazeComponent();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Daze")
	float m_fCurrentDaze;

	UPROPERTY(EditDefaultsOnly, Category = "Daze")
	float m_fMaxDaze;

	UPROPERTY(EditDefaultsOnly, Category = "Daze")
	float m_fDazeDuration;

	UPROPERTY(VisibleInstanceOnly, Category = "Daze")
	bool m_bIsDazed;

	bool m_bDazeTimerPause = false;

	FTimerHandle DazeRecoverTimerHandle;

	virtual void InitializeComponent() override;

public:
	UFUNCTION()
	float GetCurrentDaze() { return m_fCurrentDaze; }

	UFUNCTION()
	float GetMaxDaze() { return m_fMaxDaze; }

	UFUNCTION(BlueprintCallable, Category = "Daze")
	void TakeDaze(float Value);

	UFUNCTION(BlueprintCallable, Category = "Daze")
	void RecoverDaze();

	UFUNCTION(BlueprintCallable, Category = "Daze")
	bool IsDazed() const { return m_bIsDazed; }

	UFUNCTION(BlueprintCallable, Category = "Daze")
	void PauseDazeTimer(bool bPause);

	UFUNCTION(BlueprintCallable, Category = "Daze")
	void TriggerChainAttack();

	UPROPERTY(BlueprintAssignable, Category = "Daze")
	FOnDazeChanged OnDazeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Daze")
	FOnDazed OnDazed;

	UPROPERTY(BlueprintAssignable, Category = "Daze")
	FOnDazeRecovered OnDazeRecovered;

	UPROPERTY(BlueprintAssignable, Category = "Daze")
	FOnChainAttack OnChainAttack;
};
