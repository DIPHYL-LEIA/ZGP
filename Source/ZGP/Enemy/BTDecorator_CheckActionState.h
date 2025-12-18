// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "../ActionState.h"
#include "BTDecorator_CheckActionState.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTDecorator_CheckActionState : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckActionState();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere)
	EActionState m_eActionState = EActionState::IDLE;
	
};
