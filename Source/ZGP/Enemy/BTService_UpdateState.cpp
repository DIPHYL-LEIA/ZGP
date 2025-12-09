// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../ActionStateProvider.h"
#include "../ActionState.h"
#include "AIKeys.h"

UBTService_UpdateState::UBTService_UpdateState()
{
	NodeName = TEXT("Update State");
	Interval = 0.1f;
	RandomDeviation = 0.02f;
}

void UBTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	IActionStateProvider* ActionState = Cast<IActionStateProvider>(MyPawn);
	if (!ActionState) return;

	// ActionState와 Blackboard 동기화
	BB->SetValueAsBool(AIKeys::IsDead, ActionState->IsActionState(EActionState::DEAD));
	BB->SetValueAsBool(AIKeys::IsDazed, ActionState->IsActionState(EActionState::DAZED));
	BB->SetValueAsBool(AIKeys::IsHurt, ActionState->IsActionState(EActionState::HIT));
}
