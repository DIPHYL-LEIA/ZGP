// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResourceProvider.generated.h"

UINTERFACE(MinimalAPI)
class UResourceProvider : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API IResourceProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	bool HasResource(float EnergyCost, float DecibelCost) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	void ConsumeResource(float EnergyCost, float DecibelCost);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resource")
	void AddResource(float EnergyGain, float DecibelGain);
};
