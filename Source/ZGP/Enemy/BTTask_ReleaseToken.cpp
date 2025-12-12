// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ReleaseToken.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../ZGPGameState.h"
#include "../TokenManagerComponent.h"
#include "AIKeys.h"

UBTTask_ReleaseToken::UBTTask_ReleaseToken()
{
	NodeName = TEXT("Release Token");
}

EBTNodeResult::Type UBTTask_ReleaseToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false)) return EBTNodeResult::Failed;

	UWorld* World = GetWorld();
	if (!World) return EBTNodeResult::Failed;

	AZGPGameState* GameState = World->GetGameState<AZGPGameState>();
	if (!GameState) return EBTNodeResult::Failed;

	UTokenManagerComponent* TokenManagerComponent = GameState->GetTokenComponent();
	if (!TokenManagerComponent) return EBTNodeResult::Failed;

	TokenManagerComponent->ReleaseToken(Context.Pawn);

	if (Context.BB)
	{
		Context.BB->SetValueAsBool(AIKeys::HasToken, false);
	}

	return EBTNodeResult::Succeeded;
}

FString UBTTask_ReleaseToken::GetStaticDescription() const
{
	return TEXT("[BTTask_ReleaseToken] : Release Token");
}
