// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateState.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTService_UpdateState : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void UpdateActionState(UBlackboardComponent* BB, class APawn* Pawn);
	void UpdateTargetInfo(class UBlackboardComponent* BB, class AAIController* Controller, class APawn* Pawn);
};
