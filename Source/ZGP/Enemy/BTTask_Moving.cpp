// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Moving.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_Moving::UBTTask_Moving()
{
	NodeName = TEXT("Moving");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_Moving::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Roaming은 타겟 불필요
	bool bRequireTarget = (m_eMoveType != EMoveType::ROAMING);

	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, bRequireTarget))
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving: Context Failed"));
		return EBTNodeResult::Failed;

	}
	// 타겟 응시
	if (m_eMoveType != EMoveType::ROAMING && Context.Target)
	{
		Context.Controller->SetFocus(Context.Target);
	}

	FVector Destination = CalculateDestination(Context.Pawn, Context.Target);
	if (Destination.IsZero())
	{
		Context.Controller->ClearFocus(EAIFocusPriority::Gameplay);				// 왜 GamePlay?
		return EBTNodeResult::Failed;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World);
		if (NavSystem)
		{
			FNavLocation NavLocation;

			bool bFound = NavSystem->ProjectPointToNavigation(Destination, NavLocation, FVector(500.f, 500.f, 200.f));
			if (bFound)
			{
				Destination = NavLocation.Location;
			}
			else
			{
				// 실패해도 Roaming은 계속
				if (m_eMoveType != EMoveType::ROAMING)
				{
					Context.Controller->ClearFocus(EAIFocusPriority::Gameplay);
					return EBTNodeResult::Failed;
				}
			}
		}
	}

	// 이동 속도
	ApplySpeedMultiply(Context.Pawn);

	// Roaming은 Strafe 없이 일반 이동
	bool bCanStrafe = (m_eMoveType != EMoveType::ROAMING);

	// 이동
	EPathFollowingRequestResult::Type Result = Context.Controller->MoveToLocation(Destination, m_fAllowRadius, true, true, false, bCanStrafe, nullptr, true);

	if (Result == EPathFollowingRequestResult::Failed)
	{
		RestoreSpeed(Context.Pawn);
		Context.Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Failed;
	}

	if (Result == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		RestoreSpeed(Context.Pawn);
		Context.Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;
	}

	m_fElapsedTime = 0.f;

	UE_LOG(LogTemp, Log, TEXT("Moving: Started Successfully"));
	return EBTNodeResult::InProgress;
}

void UBTTask_Moving::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	m_fElapsedTime += DeltaSeconds;

	if (m_fElapsedTime >= m_fTimeout)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);		// InProgress와 짝지어줘야함
		return;
	}

	// 이동 완료
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	EPathFollowingStatus::Type Status = Context.Controller->GetMoveStatus();
	if (Status == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_Moving::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	AIController->StopMovement();

	if (APawn* Pawn = AIController->GetPawn())
	{
		RestoreSpeed(Pawn);
	}

	m_fElapsedTime = 0.f;
	m_fOriginalSpeed = 0.f;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}


FVector UBTTask_Moving::CalculateDestination(APawn* Pawn, const AActor* Target) const
{
	switch (m_eMoveType)
	{
	case EMoveType::MOVE_AWAY:
		return CalculateMoveAwayLocation(Pawn, Target);
	case EMoveType::ROAMING:
		return CalculateRoamingLocation(Pawn);
	default:
		return CalculateStrafeLocation(Pawn, Target);
	}

}

FVector UBTTask_Moving::CalculateMoveAwayLocation(APawn* Pawn, const AActor* Target) const
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

	FVector MoveLocation = MyLocation + (BackDirection * m_fMoveAwayDistance);

	return MoveLocation;
}

FVector UBTTask_Moving::CalculateStrafeLocation(APawn* Pawn, const AActor* Target) const
{
	if (!Pawn || !Target) return FVector::ZeroVector;

	FVector MyLocation = Pawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Direction = MyLocation - TargetLocation;
	Direction.Z = 0.f;

	float CurrentDistance = Direction.Size();
	float MinCombatDistance = 400.f;
	float TargetDistance = FMath::Max(CurrentDistance, MinCombatDistance);

	if (CurrentDistance < KINDA_SMALL_NUMBER)
	{
		CurrentDistance = 300.f;
		Direction = -Pawn->GetActorForwardVector();
	}
	Direction.Normalize();

	// Strafe 방향
	float StrafeDirection = 0.f;
	switch (m_eMoveType)
	{
	case EMoveType::STRAFE_LEFT:
		StrafeDirection = 1.f;
		break;
	case EMoveType::STRAFE_RIGHT:
		StrafeDirection = -1.f;
		break;
	case EMoveType::STRAFE_RANDOM:
		StrafeDirection = FMath::RandBool() ? 1.f : -1.f;
		break;
	default:
		break;
	}

	float AngleRadian = FMath::DegreesToRadians(m_fStrafeAngle * StrafeDirection);

	// 회전 행렬
	FVector RotateDirection;
	RotateDirection.X = (Direction.X * FMath::Cos(AngleRadian)) + (Direction.Y * (-FMath::Sin(AngleRadian)));
	RotateDirection.Y = (Direction.X * FMath::Sin(AngleRadian)) + (Direction.Y * FMath::Cos(AngleRadian));
	RotateDirection.Z = 0.f;

	// 타겟에서 회전된 방향으로 현재 거리만큼 떨어진 위치
	FVector MoveLocation;
	MoveLocation = TargetLocation + (RotateDirection * TargetDistance);

	return MoveLocation;
}

FVector UBTTask_Moving::CalculateRoamingLocation(APawn* Pawn) const
{
	if (!Pawn) return FVector::ZeroVector;

	FVector MyLocation = Pawn->GetActorLocation();

	float RandomAngle = FMath::FRandRange(0.f, 2.f * PI);
	float RandomDistance = FMath::FRandRange(m_fRoamingMinRadius, m_fRoamingMaxRadius);

	FVector Offset;
	Offset.X = FMath::Cos(RandomAngle) * RandomDistance;
	Offset.Y = FMath::Sin(RandomAngle) * RandomDistance;
	Offset.Z = 0.f;

	return MyLocation + Offset;
}

void UBTTask_Moving::ApplySpeedMultiply(APawn* Pawn)
{
	if (!Pawn) return;

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			m_fOriginalSpeed = MovementComponent->MaxWalkSpeed;
			MovementComponent->MaxWalkSpeed = m_fOriginalSpeed * m_fSpeedMultiply;
		}
	}
}

void UBTTask_Moving::RestoreSpeed(APawn* Pawn)
{
	if (!Pawn) return;

	if (m_fOriginalSpeed <= 0.f) return;

	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->MaxWalkSpeed = m_fOriginalSpeed;
		}
	}
}

FString UBTTask_Moving::GetStaticDescription() const
{
	return FString();
}