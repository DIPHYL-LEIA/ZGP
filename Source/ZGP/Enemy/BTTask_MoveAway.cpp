// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveAway.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_MoveAway::UBTTask_MoveAway()
{
	NodeName = TEXT("Move Away From Target");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_MoveAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, true)) return EBTNodeResult::Failed;

	FVector Destination = CalculateMove(Context.Pawn, Context.Target);

	UWorld* World = GetWorld();
	if (!World) return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
	if (!NavSystem) return EBTNodeResult::Failed;

	FNavLocation NavLocation;

	bool bFound = NavSystem->ProjectPointToNavigation(Destination, NavLocation, FVector(100.f, 100.f, 200.f));
	if (bFound)
	{
		Destination = NavLocation.Location;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	// [디버그] 목적지 표시
	if (m_bShowDebug)
	{
		DrawDebugSphere(World, Destination, 30.f, 12, FColor::Red, false, 1.0f);
		DrawDebugLine(World, Context.Pawn->GetActorLocation(), Destination, FColor::Red, false, 1.0f);
	}

	// 이동
	EPathFollowingRequestResult::Type Result = Context.Controller->MoveToLocation(Destination, m_fMoveRadius, true, true, false, true, nullptr, true);

	if (Result == EPathFollowingRequestResult::Failed)			return EBTNodeResult::Failed;
	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)	return EBTNodeResult::Succeeded;

	m_fElapsedTime = 0.f;

	return EBTNodeResult::InProgress;
}

void UBTTask_MoveAway::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	m_fElapsedTime += DeltaSeconds;

	// 타임 아웃 체크
	if (m_fElapsedTime >= m_fTimeout)
	{
		Context.Controller->StopMovement();

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// 이동 완료 체크
	EPathFollowingStatus::Type Status = Context.Controller->GetMoveStatus();
	if (Status == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

void UBTTask_MoveAway::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	m_fElapsedTime = 0.f;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

FVector UBTTask_MoveAway::CalculateMove(APawn* Pawn, AActor* Target) const
{
	if (!Pawn || !Target) return FVector::ZeroVector;

	FVector MyLocation = Pawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	// 뒤로 이동해야하므로 방향은 '나 -> 타겟'이 아닌 '타겟 -> 나'
	FVector BackDirection = MyLocation - TargetLocation;
	BackDirection.Z = 0.f;

	// 방향이 0에 가까우면 Pawn의 후방 방향으로 세팅
	if (BackDirection.IsNearlyZero())
	{
		BackDirection = -Pawn->GetActorForwardVector();
	}

	BackDirection.Normalize();
	
	FVector MoveLocation = MyLocation + (BackDirection * m_fMoveDistance);
	
	return MoveLocation;
}
