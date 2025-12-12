// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Base.h"
#include "BTTask_MoveAway.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_MoveAway : public UBTTask_Base
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveAway();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "MoveAway", meta = (ClampMin = "50.0"))
	float m_fMoveDistance = 200.f;

	UPROPERTY(EditAnywhere, Category = "MoveAway", meta = (ClampMin = "10.0"))
	float m_fMoveRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "MoveAway", meta = (ClampMin = "0.1"))
	float m_fTimeout = 3.f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool m_bShowDebug = false;


private:
	FVector CalculateMove(APawn* Pawn, AActor* Target) const;

	float m_fElapsedTime = 0.f;
};
