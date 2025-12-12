// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKeys.h"

UBTTask_RotateToTarget::UBTTask_RotateToTarget()
{
	NodeName = TEXT("Rotate To Target");
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, true)) return EBTNodeResult::Failed;

	if (UpdateRotation(Context.Pawn, Context.Target, 0.f))
	{
		return EBTNodeResult::Succeeded;
	}


	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(AIKeys::TargetActor));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (UpdateRotation(MyPawn, Target, DeltaSeconds))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

FString UBTTask_RotateToTarget::GetStaticDescription() const
{
	return FString();
}

bool UBTTask_RotateToTarget::UpdateRotation(APawn* Pawn, AActor* Target, float DeltaSeconds)
{
	if (!Pawn || !Target) return false;

	FVector MyLocation = Pawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	FVector Direction = TargetLocation - MyLocation;
	Direction.Z = 0.f;

	if (Direction.IsNearlyZero()) return true;

	Direction.Normalize();

	FRotator TargetRotation = Direction.Rotation();
	FRotator CurrentRotation = Pawn->GetActorRotation();

	float DeltaYaw = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw);

	if (FMath::Abs(DeltaYaw) <= m_fAcceptableAngle) return true;

	// DeltaSeconds가 0보다 클 때만 회전 적용
	if (DeltaSeconds > 0.f)
	{
		float AngularVel = m_fRotationSpeed * DeltaSeconds;
		float Yaw = FMath::Clamp(DeltaYaw, -AngularVel, AngularVel);

		FRotator NewRotation = CurrentRotation;
		NewRotation.Yaw += Yaw;

		Pawn->SetActorRotation(NewRotation);
	}
	return false;
}
