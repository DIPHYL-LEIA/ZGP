// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "ZGPEventManager.generated.h"

USTRUCT(BlueprintType)
struct FGameplayEventContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TWeakObjectPtr<AActor> Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
	TWeakObjectPtr<AActor> Target;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayEvent, const FGameplayEventContext&, Context);

UCLASS()
class ZGP_API UZGPEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EventManager")
	void BroadCast(FGameplayTag EventTag, const FGameplayEventContext& Context);

	FOnGameplayEvent& GetEventDelegate(FGameplayTag EventTag);

private:
	UPROPERTY()
	TMap<FGameplayTag, FOnGameplayEvent> EventDispatchers;

};
