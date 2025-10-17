// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ZGPEventManager.generated.h"

USTRUCT(BlueprintType)
struct FGameEventBase
{
	GENERATED_BODY()
};

UCLASS()
class ZGP_API UZGPEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:


private:

};
