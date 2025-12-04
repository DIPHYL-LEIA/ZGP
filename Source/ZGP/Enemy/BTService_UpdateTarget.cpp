// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../TargetProvider.h"
#include "AIKeys.h"

UBTService_UpdateTarget::UBTService_UpdateTarget()
{
	NodeName = TEXT("Update Target");
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return;

	AActor* Target = nullptr;
	if (AIController->Implements<UTargetProvider>())
	{
		Target = ITargetProvider::Execute_GetCurrentTargetActor(AIController);
	}

	BB->SetValueAsObject(AIKeys::TargetActor, Target);

	if (Target)
	{
		FVector MyLocation = MyPawn->GetActorLocation();
		FVector TargetLocation = Target->GetActorLocation();

		// 거리
		float Distance = FVector::Dist(MyLocation, TargetLocation);
		BB->SetValueAsFloat(AIKeys::TargetDistance, Distance);

		// 각도
		FVector Forward = MyPawn->GetActorForwardVector();
		FVector TargetDirection = (TargetLocation - MyLocation).GetSafeNormal();
		float AngleDot = FVector::DotProduct(Forward, TargetDirection);
		BB->SetValueAsFloat(AIKeys::TargetAngle, AngleDot);
	}
	else
	{
		BB->SetValueAsFloat(AIKeys::TargetDistance, 0.f);
		BB->SetValueAsFloat(AIKeys::TargetAngle, 0.f);
	}

}
