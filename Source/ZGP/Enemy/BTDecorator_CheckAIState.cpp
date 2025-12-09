// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckAIState.h"
#include "AIController.h"
#include "../ActionStateProvider.h"

UBTDecorator_CheckAIState::UBTDecorator_CheckAIState()
{
	NodeName = TEXT("Check Action State");
}

bool UBTDecorator_CheckAIState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return false;

	IActionStateProvider* ActionState = Cast<IActionStateProvider>(MyPawn);
	if (!ActionState) return false;

	return ActionState->IsActionState(m_eState);
}

FString UBTDecorator_CheckAIState::GetStaticDescription() const
{
	FString StateName = UEnum::GetDisplayValueAsText(m_eState).ToString();
	return FString::Printf(TEXT("Action State = %s"), *StateName);
}
