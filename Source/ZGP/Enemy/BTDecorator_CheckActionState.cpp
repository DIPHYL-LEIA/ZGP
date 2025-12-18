// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CheckActionState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CheckActionState::UBTDecorator_CheckActionState()
{
	NodeName = TEXT("Check Action State");

	// Observer Aborts 지원 세팅
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

bool UBTDecorator_CheckActionState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	// "ActionState" 키 값을 읽음
	uint8 CurrentState = BB->GetValueAsEnum(TEXT("ActionState"));

	return CurrentState == static_cast<uint8>(m_eActionState);
}

FString UBTDecorator_CheckActionState::GetStaticDescription() const
{
	FString StateName = UEnum::GetDisplayValueAsText(m_eActionState).ToString();
	return FString::Printf(TEXT("Blackboard['ActionState'] == %s"), *StateName);
}
