// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemySkillComponent.h"
#include "AIKeys.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bCreateNodeInstance = true;				// 다수의 AI 사용 시 멤버 변수 충돌 방지
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTContext Context;
	if (!GetBTContext(OwnerComp, Context, false)) return EBTNodeResult::Failed;

	FName Skill = m_SkillID;

	if (Skill.IsNone() && m_BlackboardKey.IsSet())
	{
		if (Context.BB)
		{
			Skill = Context.BB->GetValueAsName(m_BlackboardKey.SelectedKeyName);
		}
	}

	if (Skill.IsNone()) return EBTNodeResult::Failed;

	// EnemySkillComponent
	UEnemySkillComponent* EnemySkillComponent = GetEnemySkillComponent(Context.Pawn);
	if (!EnemySkillComponent) return EBTNodeResult::Failed;

	m_pCacheBTComponent = &OwnerComp;
	m_pCachedPawn = Context.Pawn;

	// 델리게이트 바인딩
	EnemySkillComponent->OnAttackCompleted.AddDynamic(this, &UBTTask_ExecuteAttack::HandleAttackCompleted);


	// 스킬 실행 요청
	bool bSuccess = EnemySkillComponent->ExecuteAttack(Skill);
	if (!bSuccess)
	{
		UnbindDelegate(Context.Pawn);
		m_pCacheBTComponent.Reset();
		m_pCachedPawn.Reset();
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::HandleAttackCompleted()
{
	if (m_pCacheBTComponent.IsValid() && m_pCachedPawn.IsValid())
	{
		UnbindDelegate(m_pCachedPawn.Get());
		FinishLatentTask(*m_pCacheBTComponent.Get(), EBTNodeResult::Succeeded);
	}

	m_pCacheBTComponent.Reset();
	m_pCachedPawn.Reset();
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// 피격으로 BT가 강제 중단될 때 델리게이트 해제
	if (m_pCachedPawn.IsValid())
	{
		UnbindDelegate(m_pCachedPawn.Get());
	}

	m_pCacheBTComponent.Reset();
	m_pCachedPawn.Reset();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

UEnemySkillComponent* UBTTask_ExecuteAttack::GetEnemySkillComponent(APawn* Pawn) const
{
	if (!Pawn) return nullptr;
	return Pawn->FindComponentByClass<UEnemySkillComponent>();
}

void UBTTask_ExecuteAttack::UnbindDelegate(APawn* Pawn)
{
	if (!Pawn) return;
	UEnemySkillComponent* EnemySkillComponent = GetEnemySkillComponent(Pawn);
	if (EnemySkillComponent)
	{
		EnemySkillComponent->OnAttackCompleted.RemoveDynamic(this, &UBTTask_ExecuteAttack::HandleAttackCompleted);
	}
}

FString UBTTask_ExecuteAttack::GetStaticDescription() const
{
	if (!m_SkillID.IsNone())
	{
		return FString::Printf(TEXT("Skill: %s (Wait)"), *m_SkillID.ToString());
	}
	if (m_BlackboardKey.IsSet())
	{
		return FString::Printf(TEXT("Skill from BB: %s (Wait)"), *m_BlackboardKey.SelectedKeyName.ToString());
	}
	return TEXT("Skill: [Not Set]");
}
