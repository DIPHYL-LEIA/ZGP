// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ReleaseToken.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_ReleaseToken : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ReleaseToken();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;


	
};
