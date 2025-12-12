// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StopMove.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_StopMove::UBTTask_StopMove()
{
	NodeName = TEXT("Stop Movement");
}

EBTNodeResult::Type UBTTask_StopMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false)) return EBTNodeResult::Failed;

	Context.Controller->StopMovement();

	return EBTNodeResult::Succeeded;
}

FString UBTTask_StopMove::GetStaticDescription() const
{
	return TEXT("Stop AI Movement");
}
