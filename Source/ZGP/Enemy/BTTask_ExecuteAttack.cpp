// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../SkillComponent.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bNotifyTaskFinished = true;				// 비용이 많이 드는 작업?
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	return EBTNodeResult::Type();
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}

FString UBTTask_ExecuteAttack::GetStaticDescription() const
{
	return FString();
}

void UBTTask_ExecuteAttack::HandleSkillCompleted()
{
}
