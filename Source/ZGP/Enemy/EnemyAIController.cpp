// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../Targetable.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

#include "../ActionStateComponent.h"		// 바인딩 해제용

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

AActor* AEnemyAIController::GetCurrentTargetActor_Implementation()
{
	return GetCurrentTarget();
}

void AEnemyAIController::SetCurrentTargetActor_Implementation(AActor* NewTarget)
{
	m_pCachedTarget = NewTarget;
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ClearTarget();

	if (InPawn)
	{
		if (UActionStateComponent* ActionStateComponent = InPawn->FindComponentByClass<UActionStateComponent>())
		{
			ActionStateComponent->OnStateChanged.AddDynamic(this, &AEnemyAIController::HandleActionStateChange);
			m_pActionStateComponent = ActionStateComponent;
		}
	}

	if (m_pBehaviorTree)
	{
		RunBehaviorTree(m_pBehaviorTree);

		if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsEnum(TEXT("ActionState"), static_cast<uint8>(EActionState::IDLE));
		}
	}
}

void AEnemyAIController::OnUnPossess()
{
	// 델리게이트 해제
	if (m_pActionStateComponent.IsValid())
	{
		m_pActionStateComponent->OnStateChanged.RemoveDynamic(this, &AEnemyAIController::HandleActionStateChange);
		m_pActionStateComponent.Reset();
	}

	Super::OnUnPossess();
}


AActor* AEnemyAIController::GetCurrentTarget()
{
	if (m_pCachedTarget.IsValid())
	{
		AActor* Target = m_pCachedTarget.Get();

		if (IsTargetValid(Target))
		{
			return Target;
		}

		// 타겟 무효화
		m_pCachedTarget.Reset();
	}

	// 연속 탐색 방지용 쿨타임 체크
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (World->GetTimeSeconds() < m_fSearchCooldownTimer)
	{
		return nullptr;
	}

	AActor* NewTarget = FindNewTarget();

	if (NewTarget)
	{
		m_pCachedTarget = NewTarget;
	}
	else
	{
		// 탐색 실패 시 쿨타임 적용
		m_fSearchCooldownTimer = World->GetTimeSeconds() + m_fSearchCooldown;
	}

	return NewTarget;
}

AActor* AEnemyAIController::FindNewTarget() const
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FVector MyLocation = MyPawn->GetActorLocation();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyPawn);

	bool bHit = World->OverlapMultiByObjectType(OverlapResults, MyLocation, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(m_fSearchRadius), QueryParams);
	if (!bHit) return nullptr;

	// 가장 가까운 타겟 선정
	AActor* BestTarget = nullptr;
	float MinDistanceSquared = FLT_MAX;						//////////////

	for (const FOverlapResult& Result : OverlapResults)
	{
		AActor* Actor = Result.GetActor();

		if (!Actor || Actor == MyPawn) continue;

		if (!Actor->Implements<UTargetable>()) continue;
		if (Actor->GetClass()->IsChildOf(MyPawn->GetClass())) continue;		// 타겟 본인 검사 이 방법 뿐?

		if (!ITargetable::Execute_IsTargetable(Actor)) continue;

		float DistanceSquared = FVector::DistSquared(MyLocation, Actor->GetActorLocation());
		if (DistanceSquared < MinDistanceSquared)
		{
			MinDistanceSquared = DistanceSquared;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

void AEnemyAIController::ClearTarget()
{
	m_pCachedTarget.Reset();
	m_fSearchCooldownTimer = 0.f;
}

void AEnemyAIController::HandleActionStateChange(EActionState OldState, EActionState NewState)
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsEnum(TEXT("ActionState"), static_cast<uint8>(NewState));
	if (NewState == EActionState::DAZED ||
		NewState == EActionState::HIT ||
		NewState == EActionState::DEAD)
	{
		StopMovement();
	}


}

bool AEnemyAIController::IsTargetValid(AActor* Target) const
{
	if (!Target) return false;

	if (!Target->Implements<UTargetable>()) return false;
	if (!ITargetable::Execute_IsTargetable(Target)) return false;

	if (m_bUseMaxAggroRange)
	{
		APawn* MyPawn = GetPawn();
		if (!MyPawn) return false;

		float DistanceSquared = FVector::DistSquared(MyPawn->GetActorLocation(), Target->GetActorLocation());

		if (DistanceSquared > FMath::Square(m_fMaxAggroRange)) return false;
	}

	return true;
}
