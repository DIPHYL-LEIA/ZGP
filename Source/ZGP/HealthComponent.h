// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChanged, AActor*, Character, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, Character);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Health")
	float m_fCurrentHealth;

	UPROPERTY(VisibleInstanceOnly, Category = "Health")
	float m_fMaxHealth;

	virtual void InitializeComponent() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageValue, AActor* Character);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDied OnDied;
};
