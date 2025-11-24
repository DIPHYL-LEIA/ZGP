// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageData.h"
#include "CombatInteraction.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInteraction : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ICombatInteraction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void ApplyCombatEffect(const FDamageData& DamageData);

	// Enemy가 눈 번쩍이는 타이밍에 사용
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	bool CanParry() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void Parried(AActor* ParryCharacter);
};
