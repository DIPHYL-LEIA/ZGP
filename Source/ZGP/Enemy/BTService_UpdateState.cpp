// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

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

	APawn* ControlPawn = AIController->GetPawn();
	if (!ControlPawn) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	// 현재 상태 확인
	EEnemyAIState CurrentState = static_cast<EEnemyAIState>(Blackboard->GetValueAsEnum(m_AIStateKey.SelectedKeyName));

	if (CurrentState == EEnemyAIState::DEAD ||
		CurrentState == EEnemyAIState::DAZED ||
		CurrentState == EEnemyAIState::HIT)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;

	if (!PlayerPawn)
	{
		Blackboard->ClearValue(m_TargetActorKey.SelectedKeyName);
		Blackboard->SetValueAsEnum(m_AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::IDLE));
		return;
	}

	FVector MyLocation = ControlPawn->GetActorLocation();
	FVector TargetLocation = PlayerPawn->GetActorLocation();
	float Distance = FVector::Dist(MyLocation, TargetLocation);

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(m_TargetActorKey.SelectedKeyName));

}
