// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnergyChanged, float, CurrentEnergy, float, MaxEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDecibelChanged, float, CurrentDecibel, float, MaxDecibel);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceComponent();

	// Á¶È¸
	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetCurrentEnergy() const { return m_fCurrentEnergy; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetMaxEnergy() const { return m_fMaxEnergy; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetCurrentDecibel() const { return m_fCurrentDecibel; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	float GetMaxDecibel() const { return m_fMaxDecibel; }

	UFUNCTION(BlueprintPure, Category = "Resource")
	bool HasEnoughEnergy(float Cost) const;

	UFUNCTION(BlueprintPure, Category = "Resource")
	bool HasEnoughDecibel(float Cost) const;

	// Cost
	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool ConsumeEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	bool ConsumeDecibel(float Amount);

	// Add
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void AddEnergy(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resource")
	void AddDecibel(float Amount);

	// Reset
	UFUNCTION(BlueprintCallable, Category = "Resource")
	void ResetDecibel();

	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Resource")
	FOnEnergyChanged OnEnergyChanged;

	UPROPERTY(BlueprintAssignable, Category = "Resource")
	FOnDecibelChanged OnDecibelChanged;

protected:
	virtual void InitializeComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	float m_fMaxEnergy = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	float m_fMaxDecibel = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	float m_fCurrentEnergy = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Resource")
	float m_fCurrentDecibel = 0.f;		
};
