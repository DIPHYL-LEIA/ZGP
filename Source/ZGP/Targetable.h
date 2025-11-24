// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Targetable.generated.h"

UINTERFACE(MinimalAPI)
class UTargetable : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ITargetable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	bool IsTargetable() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	FVector GetTargetLocation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	void OnTargeted(bool IsTargeted);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	void OnUnTargeted();
};
