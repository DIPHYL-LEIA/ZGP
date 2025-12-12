// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Base.h"
#include "BTTask_ExecuteAttack.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UBTTask_ExecuteAttack : public UBTTask_Base
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	FName m_SkillID;

	UPROPERTY(EditAnywhere, Category = "Attack")
	FBlackboardKeySelector  m_BlackboardKey;

private:
	UFUNCTION()
	void HandleSkillCompleted();

	class USkillComponent* GetSkillComponent(UBehaviorTreeComponent& OwnerComp) const;

	void UnbindSkillDelegate(UBehaviorTreeComponent& OwnerComp);

	TWeakObjectPtr<UBehaviorTreeComponent> m_pCacheBTComponent;

};
