// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionState.h"
#include "ComboComponent.generated.h"

// Combo Step
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerformComboAttack, int32, ComboStep);
DECLARE_DELEGATE(FOnResetCombo);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Combo", meta = (AllowPrivateAccess = "true"))
	int32 m_nCurrentComboStep;

	UPROPERTY(VisibleInstanceOnly, Category = "Combo", meta = (AllowPrivateAccess = "true"))
	bool m_bIsComboWindowStart;

	UPROPERTY(VisibleInstanceOnly, Category = "Combo", meta = (AllowPrivateAccess = "true"))
	float m_fComboResetTime;

	FTimerHandle ComboResetTimer;

public:
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnPerformComboAttack	OnPerformComboAttack;
	FOnResetCombo			OnResetCombo;

protected:
	virtual void BeginPlay() override;

public:
	UComboComponent();
	void RequestComboAttack();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void StartComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();

private:
	class IActionStateProvider* GetActionStateProvider() const;
	void HandleComboReset();

};
