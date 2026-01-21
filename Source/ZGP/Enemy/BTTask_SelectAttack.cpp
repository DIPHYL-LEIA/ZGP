// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelectAttack.h"
//#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIKeys.h"
#include "EnemyAttackSelectorComponent.h"
#include "../EnemyCharacter.h"


UBTTask_SelectAttack::UBTTask_SelectAttack()
{
	NodeName = TEXT("Select Attack");
}

EBTNodeResult::Type UBTTask_SelectAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false)) return EBTNodeResult::Failed;

	float Distance = Context.BB->GetValueAsFloat(AIKeys::TargetDistance);
	if (Distance < 0.5)
	{
		if (!m_FallbackAttackID.IsNone())
		{
			Context.BB->SetValueAsName(AIKeys::SelectAttackID, m_FallbackAttackID);
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::Failed;
	}

	UEnemyAttackSelectorComponent* EnemyAttackSelectorComponenet = GetAttackSelector(Context.Pawn);
	if (!EnemyAttackSelectorComponenet) return EBTNodeResult::Failed;

	EEnemyType EnemyType = m_bOverrideEnemyType ? m_eEnemyType : GetEnemyType(Context.Pawn);

	FName SelectedAttakID = NAME_None;

	switch (EnemyType)
	{
	case EEnemyType::NORMAL:
		SelectedAttakID = SelectAttackNormal(EnemyAttackSelectorComponenet, Distance);
		break;
	case EEnemyType::ELITE:
		SelectedAttakID = SelectAttackElite(EnemyAttackSelectorComponenet, Distance);
		break;
	case EEnemyType::BOSS:
		SelectedAttakID = SelectAttackBoss(EnemyAttackSelectorComponenet, Distance);
		break;
	default:
		SelectedAttakID = SelectAttackNormal(EnemyAttackSelectorComponenet, Distance);
		break;
	}

	// 선택 실패 Fallback
	if (SelectedAttakID.IsNone())
	{
		if (!m_FallbackAttackID.IsNone())
		{
			SelectedAttakID = m_FallbackAttackID;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}

	// Blackboard에 저장
	Context.BB->SetValueAsName(AIKeys::SelectAttackID, SelectedAttakID);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_SelectAttack::GetStaticDescription() const
{
	if (m_bOverrideEnemyType)
	{
		FString TypeName = UEnum::GetDisplayValueAsText(m_eEnemyType).ToString();
		return FString::Printf(TEXT("Select Attack ( %s )"), *TypeName);
	}

	FString Desc = TEXT("Select Attack (Auto by EnemyType)");
	if (!m_FallbackAttackID.IsNone())
	{
		Desc += FString::Printf(TEXT("\nFallback: %s"), *m_FallbackAttackID.ToString());
	}
	return Desc;
}

FName UBTTask_SelectAttack::SelectAttackNormal(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	FAttackSelectResult Result = Selector->SelectAttackDistance(Distance);
	return Result.bSuccess ? Result.SelectAttackID : NAME_None;
}

FName UBTTask_SelectAttack::SelectAttackElite(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	float HPRatio = Selector->GetCurrentHPRatio();

	if (HPRatio <= 0.3f)
	{
		FEnemyAttackPattern Pattern;
		if (Selector->GetPattern(Pattern) && Pattern.EnrageAttack.Num() > 0)
		{
			// Enrage 중 사용 가능한 것 필터링
			TArray<FName> Available = Selector->GetAvailableAttack(Pattern.EnrageAttack, Distance, HPRatio, Selector->GetCurrentPhase());
			if (Available.Num() > 0)
			{
				FAttackSelectResult Result = Selector->SelectAttackRandom(Available);
				if (Result.bSuccess)
				{
					return Result.SelectAttackID;
				}
			}
		}
	}
	
	// Enrage 없으면 거리 기반
	FAttackSelectResult Result = Selector->SelectAttackDistance(Distance);
	return Result.bSuccess ? Result.SelectAttackID : NAME_None;
}

FName UBTTask_SelectAttack::SelectAttackBoss(UEnemyAttackSelectorComponent* Selector, float Distance)
{
	if (!Selector) return NAME_None;

	int32 CurrentPhase = Selector->GetCurrentPhase();
	FAttackSelectResult Result = Selector->SelectAttackPhase(CurrentPhase, Distance);

	if (Result.bSuccess)
	{
		return Result.SelectAttackID;
	}

	// Phase 없으면 거리 기반 Fallback
	Result = Selector->SelectAttackDistance(Distance);
	return Result.bSuccess ? Result.SelectAttackID : NAME_None;
}

UEnemyAttackSelectorComponent* UBTTask_SelectAttack::GetAttackSelector(APawn* Pawn) const
{
	if (!Pawn) return nullptr;
	return Pawn->FindComponentByClass<UEnemyAttackSelectorComponent>();
}

EEnemyType UBTTask_SelectAttack::GetEnemyType(APawn* Pawn) const
{
	if (!Pawn) return EEnemyType::NORMAL;

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Pawn);
	if (Enemy) return Enemy->GetEnemyType();

	return EEnemyType::NORMAL;
}
