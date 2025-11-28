// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../ActionState.h"
#include "EnemyAIState.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	UFUNCTION(BlueprintPure, Category = "AI")
	EEnemyAIState GetCurrentAIState() const;

	// ActionState 기반 행동 가능 여부
	UFUNCTION(BlueprintPure, Category = "AI")
	bool CanAct() const;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void HandleActionStateChanged(EActionState OldState, EActionState NewState);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> m_pBehaviorTree;


	// Blackboard key name	: 전역 변수로 사용하여 FName으로 변환하는 비용 들이지 않아도 됨
	static const FName BB_TargetActor;
	static const FName BB_AIState;
	static const FName BB_CanAct;

};
