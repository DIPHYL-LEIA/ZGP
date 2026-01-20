// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_Base.h"
#include "EnemyType.h"
#include "BTTask_SelectAttack.generated.h"

/**
 * Normal	: 거리 기반
 * Elite	: 거리 + HP
 * Boss		: Phase 기반 (HP 구간별 패턴)
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

	UPROPERTY(EditAnywhere, Category = "Select Mode")
	bool m_bSelectMode = false;

	UPROPERTY(EditAnywhere, Category = "Select Mode")
	EEnemyType m_eEnemyType = EEnemyType::NORMAL;

	UPROPERTY(EditAnywhere, Category = "Select Mode")
	FName m_FallbackAttakID = NAME_None;

private:
	FName SelectAttackNormal(class UEnemyAttackSelectorComponent* Selector, float Distance);
	FName SelectAttackElite(class UEnemyAttackSelectorComponent* Selector, float Distance);
	FName SelectAttackBoss(class UEnemyAttackSelectorComponent* Selector, float Distance);

	class UEnemyAttackSelectorComponent* GetAttackSelector(UBehaviorTreeComponent& OwnerComp) const;
	EEnemyType GetEnemyType(APawn* Pawn) const;
};
