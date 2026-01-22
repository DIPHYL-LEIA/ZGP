// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateState.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../ActionStateProvider.h"
#include "../ActionState.h"
#include "AIKeys.h"
#include "../TargetProvider.h"

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

	UpdateActionState(BB, MyPawn);
	UpdateTargetInfo(BB, AIController, MyPawn);
}

void UBTService_UpdateState::UpdateActionState(UBlackboardComponent* BB, APawn* Pawn)
{
	if (!BB || !Pawn) return;

	IActionStateProvider* StateProvider = Cast<IActionStateProvider>(Pawn);
	if (!StateProvider) return;

	// ActionState와 Blackboard 동기화
	BB->SetValueAsBool(AIKeys::IsDead, StateProvider->IsActionState(EActionState::DEAD));
	BB->SetValueAsBool(AIKeys::IsDazed, StateProvider->IsActionState(EActionState::DAZED));
	BB->SetValueAsBool(AIKeys::IsHurt, StateProvider->IsActionState(EActionState::HIT));
}

void UBTService_UpdateState::UpdateTargetInfo(UBlackboardComponent* BB, AAIController* Controller, APawn* Pawn)
{
	if (!BB || !Controller || !Pawn) return;

	AActor* Target = nullptr;

	if (Controller->Implements<UTargetProvider>())
	{
		Target = ITargetProvider::Execute_GetCurrentTargetActor(Controller);
	}

	BB->SetValueAsObject(AIKeys::TargetActor, Target);

	// 타겟이 없으면 거리/각도 초기화
	if (!Target)
	{
		BB->SetValueAsFloat(AIKeys::TargetDistance, 0.f);
		BB->SetValueAsFloat(AIKeys::TargetAngle, 0.f);
		return;
	}

	// 거리 계산
	FVector MyLocation = Pawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	float Distance = FVector::Dist(MyLocation, TargetLocation);
	BB->SetValueAsFloat(AIKeys::TargetDistance, Distance);

	// 정면 기준 (-180 ~ 180)
	FVector DirectionTarget = TargetLocation - MyLocation;
	DirectionTarget.Z = 0.f;

	if (!DirectionTarget.IsNearlyZero())
	{
		DirectionTarget.Normalize();

		FVector Forward = Pawn->GetActorForwardVector();
		Forward.Z = 0.f;
		Forward.Normalize();

		float DotProduct = FVector::DotProduct(Forward, DirectionTarget);
		float AngleRadian = FMath::Acos(FMath::Clamp(DotProduct, -1.f, 1.f));
		float AngleDegree = FMath::RadiansToDegrees(AngleRadian);

		// 좌우 판별
		FVector CrossProduct = FVector::CrossProduct(Forward, DirectionTarget);
		if (CrossProduct.Z > 0)
		{
			AngleDegree = -AngleDegree;
		}
		BB->SetValueAsFloat(AIKeys::TargetAngle, AngleDegree);
	}
	else
	{
		BB->SetValueAsFloat(AIKeys::TargetAngle, 0.f);
	}

}
