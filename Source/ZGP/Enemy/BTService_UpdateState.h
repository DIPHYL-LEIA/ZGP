// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "EnemyAIState.h"
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

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector m_TargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector m_AIStateKey;

	UPROPERTY(EditAnywhere)
	float m_fAttackRange = 200.f;

	UPROPERTY(EditAnywhere)
	float m_fDetectRange = 1500.f;

	UPROPERTY(EditAnywhere)
	float m_fCancelRange = 1700.f;
};
