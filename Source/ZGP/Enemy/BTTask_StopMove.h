// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Base.h"
#include "BTTask_StopMove.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_StopMove : public UBTTask_Base
{
	GENERATED_BODY()

public:
	UBTTask_StopMove();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
};
