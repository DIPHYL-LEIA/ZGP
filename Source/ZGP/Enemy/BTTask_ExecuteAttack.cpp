// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ExecuteAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../SkillComponent.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bCreateNodeInstance = true;				// 다수의 AI 사용 시 멤버 변수 충돌 방지
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	USkillComponent* SkillComponent = GetSkillComponent(OwnerComp);
	if (!SkillComponent) return EBTNodeResult::Failed;

	FName Skill = m_SkillID;

	if (Skill.IsNone() && m_BlackboardKey.IsSet())
	{
		UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
		if (BB)
		{
			Skill = BB->GetValueAsName(m_BlackboardKey.SelectedKeyName);
		}
	}

	if (Skill.IsNone()) return EBTNodeResult::Failed;

	// 델리게이트 바인딩
	m_pCacheBTComponent = &OwnerComp;
	SkillComponent->OnSkillCompleted.AddDynamic(this, &UBTTask_ExecuteAttack::HandleSkillCompleted);

	// 스킬 실행 요청
	bool bStart = SkillComponent->ExecuteSkillID(Skill);
	if (!bStart)
	{
		SkillComponent->OnSkillCompleted.RemoveDynamic(this, &UBTTask_ExecuteAttack::HandleSkillCompleted);
		m_pCacheBTComponent.Reset();
		return EBTNodeResult::Failed;

	}
	return EBTNodeResult::InProgress;
}

void UBTTask_ExecuteAttack::HandleSkillCompleted()
{
	// Task Instance 유효성 확인
	if (m_pCacheBTComponent.IsValid())
	{
		UBehaviorTreeComponent* OwnerComp = m_pCacheBTComponent.Get();
		// 정리
		UnbindSkillDelegate(*OwnerComp);
		// 트리 복귀
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// 피격으로 BT가 강제 중단될 때 델리게이트 해제
	UnbindSkillDelegate(OwnerComp);
	m_pCacheBTComponent.Reset();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

USkillComponent* UBTTask_ExecuteAttack::GetSkillComponent(UBehaviorTreeComponent& OwnerComp) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		APawn* MyPawn = AIController->GetPawn();
		if (MyPawn)
		{
			return MyPawn->FindComponentByClass<USkillComponent>();
		}
	}
	return nullptr;
}

void UBTTask_ExecuteAttack::UnbindSkillDelegate(UBehaviorTreeComponent& OwnerComp)
{
	USkillComponent* SkillComponent = GetSkillComponent(OwnerComp);
	if (SkillComponent)
	{
		SkillComponent->OnSkillCompleted.RemoveDynamic(this, &UBTTask_ExecuteAttack::HandleSkillCompleted);
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
