// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetProvider.generated.h"

UINTERFACE(MinimalAPI)
class UTargetProvider : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ITargetProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	AActor* GetCurrentTargetActor() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
	void SetCurrentTargetActor(AActor* NewTarget);
};
