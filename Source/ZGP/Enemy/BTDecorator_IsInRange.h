// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInRange.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTDecorator_IsInRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_IsInRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Range", meta = (ClampMin = "0.0"))
	float m_fMinDistance = 0.f;

	UPROPERTY(EditAnywhere, Category = "Range", meta = (ClampMin = "0.0"))
	float m_fMaxDistance = 200.f;

};
