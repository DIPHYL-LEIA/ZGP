// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dazeable.generated.h"

UINTERFACE(MinimalAPI)
class UDazeable : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API IDazeable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void PauseDazeTimer(bool bPause);

	UFUNCTION(BlueprintNativeEvent)
	bool IsDazed() const;

};
