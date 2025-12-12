// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTContext.h"
#include "BTTask_Base.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ZGP_API UBTTask_Base : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_Base();

protected:
	bool GetBTContext(UBehaviorTreeComponent& OwnerComp, FBTContext& OutContext, bool bTarget = false) const;

};
