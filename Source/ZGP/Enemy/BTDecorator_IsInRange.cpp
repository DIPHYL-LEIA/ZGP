// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKeys.h"

UBTDecorator_IsInRange::UBTDecorator_IsInRange()
{
	NodeName = TEXT("Is In Range");
}

bool UBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	UObject* TargetObject = BB->GetValueAsObject(AIKeys::TargetActor);
	if (!TargetObject) return false;

	float Distance = BB->GetValueAsFloat(AIKeys::TargetDistance);

	return (Distance >= m_fMinDistance) && (Distance <= m_fMaxDistance);
}

FString UBTDecorator_IsInRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("Distance : %.0f ~ %.0f"), m_fMinDistance, m_fMaxDistance);
}
