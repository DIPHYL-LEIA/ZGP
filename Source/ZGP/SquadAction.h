// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SquadAction.generated.h"

UINTERFACE(MinimalAPI)
class USquadAction : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ISquadAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SquadCombat")
	bool TryTriggerChainAttack(AActor* DazedEnemy);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SquadCombat")
	bool TryTriggerParryAssist(AActor* Attacker, const FVector& AttackDirection);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SquadCombat")
	bool TryTriggerQuickAssist();


};
