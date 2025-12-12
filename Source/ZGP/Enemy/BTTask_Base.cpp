// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Base.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKeys.h"

UBTTask_Base::UBTTask_Base()
{
}

bool UBTTask_Base::GetBTContext(UBehaviorTreeComponent& OwnerComp, FBTContext& OutContext, bool bTarget) const
{
	OutContext.Controller = OwnerComp.GetAIOwner();
	if (!OutContext.Controller) return false;

	OutContext.Pawn = OutContext.Controller->GetPawn();
	if (!OutContext.Pawn) return false;

	OutContext.BB = OwnerComp.GetBlackboardComponent();
	if (!OutContext.BB) return false;

	OutContext.Target = Cast<AActor>(OutContext.BB->GetValueAsObject(AIKeys::TargetActor));

	if (bTarget && !OutContext.Target) return false;

	return true;
}
