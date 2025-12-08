// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RequestToken.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../ZGPGameState.h"
#include "../TokenManagerComponent.h"
#include "AIKeys.h"

UBTTask_RequestToken::UBTTask_RequestToken()
{
	NodeName = TEXT("Request Token");
}

EBTNodeResult::Type UBTTask_RequestToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UWorld* World = GetWorld();
	if (!World) return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	AZGPGameState* GameState = World->GetGameState<AZGPGameState>();
	if (!GameState) return EBTNodeResult::Failed;

	UTokenManagerComponent* TokenManagerComponent = GameState->GetTokenComponent();
	if (!TokenManagerComponent) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	if (TokenManagerComponent->HasToken(MyPawn))
	{
		BB->SetValueAsBool(AIKeys::HasToken, true);
		return EBTNodeResult::Succeeded;
	}

	bool Success = TokenManagerComponent->RequestToken(MyPawn, ETokenPriority::NORMAL);
	BB->SetValueAsBool(AIKeys::HasToken, Success);

	return Success ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

FString UBTTask_RequestToken::GetStaticDescription() const
{
	return TEXT("[BTTask_RequestToken] : Request Token from TokenManagerComponent");
}
