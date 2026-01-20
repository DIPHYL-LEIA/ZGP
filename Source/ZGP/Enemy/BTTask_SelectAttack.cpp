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
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false))
	{
		return EBTNodeResult::Type();
	}

	UEnemyAttackSelectorComponent* AttackSelector = GetAttackSelector(OwnerComp);
	if (!AttackSelector)
	{
		return m_bSucceedNoSelect ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	FAttackSelectResult Result;

	switch (m_eSelectMode)
	{
	case EEnemyAttackSelectMode::DISTANCE_BASE:
	{
		float Distance = GetDistanceTarget(Context);
		Result = AttackSelector->SelectAttackDistance(Distance);
	}
	break;
	case EEnemyAttackSelectMode::RANDOM_WEIGHT:
	{

	}
		break;
	case EEnemyAttackSelectMode::STEP_BASE:
		break;
	case EEnemyAttackSelectMode::PHASE_BASE:
		break;
	case EEnemyAttackSelectMode::PRIORITY:
		break;
	case EEnemyAttackSelectMode::MAX:
		break;
	default:
		break;
	}

}

FString UBTTask_SelectAttack::GetStaticDescription() const
{
	return FString();
}

UEnemyAttackSelectorComponent* UBTTask_SelectAttack::GetAttackSelector(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return nullptr;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return nullptr;

	return Pawn->FindComponentByClass<UEnemyAttackSelectorComponent>();
}

float UBTTask_SelectAttack::GetDistanceTarget(const FBTContext& Context) const
{
	if (!Context.Pawn || !Context.Target)
		return 0.0f;

	return FVector::Dist(Context.Pawn->GetActorLocation(), Context.Target->GetActorLocation());
}
