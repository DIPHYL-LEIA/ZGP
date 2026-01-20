// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_Base.h"
#include "EnemyAttackData.h"
#include "BTTask_SelectAttack.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_SelectAttack : public UBTTask_Base
{
	GENERATED_BODY()
	
public:
	UBTTask_SelectAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Selection")
	EEnemyAttackSelectMode m_eSelectMode = EEnemyAttackSelectMode::DISTANCE_BASE;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector m_SelectedAttackKey;

	// 공격 선택 안 해도 트리는 계속 진행할지 여부
	UPROPERTY(EditAnywhere, Category = "Behavior")
	bool m_bSucceedNoSelect = false;

private:
	class UEnemyAttackSelectorComponent* GetAttackSelector(UBehaviorTreeComponent& OwnerComp) const;
	float GetDistanceTarget(const FBTContext& Context) const;
};
