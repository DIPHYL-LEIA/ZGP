// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAttackSelectorComponent.h"
#include "AIKeys.h"

UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("Select Attack");
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

}

FString UBTTask_SelectAttack::GetStaticDescription() const
{
	return FString();
}

FName UBTTask_SelectAttack::SelectAttackNormal(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	return FName();
}

FName UBTTask_SelectAttack::SelectAttackElite(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	return FName();
}

FName UBTTask_SelectAttack::SelectAttackBoss(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	return FName();
}

UEnemyAttackSelectorComponent* UBTTask_SelectAttack::GetAttackSelector(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return nullptr;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return nullptr;

	return Pawn->FindComponentByClass<UEnemyAttackSelectorComponent>();
}
