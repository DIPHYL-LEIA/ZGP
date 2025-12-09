// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "../ActionState.h"
#include "BTDecorator_CheckAIState.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTDecorator_CheckAIState : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckAIState();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "ActionState")
	EActionState m_eState = EActionState::IDLE;
};
