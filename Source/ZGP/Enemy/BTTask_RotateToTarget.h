// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Base.h"
#include "BTTask_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_RotateToTarget : public UBTTask_Base
{
	GENERATED_BODY()

public:
	UBTTask_RotateToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Rotate", meta = (ClampMin = "0.0"))
	float m_fRotationSpeed = 360.f;

	UPROPERTY(EditAnywhere, Category = "Rotate", meta = (ClampMin = "0.0"))
	float m_fAcceptableAngle = 5.f;

private:
	bool UpdateRotation(APawn* Pawn, AActor* Target, float DeltaSeconds);
};
