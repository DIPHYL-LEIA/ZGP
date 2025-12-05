// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../TargetProvider.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API AEnemyAIController : public AAIController, public ITargetProvider
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

	virtual AActor* GetCurrentTargetActor_Implementation() override;
	virtual void SetCurrentTargetActor_Implementation(AActor* NewTarget) override;

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	AActor* GetCurrentTarget();

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	AActor* FindNewTarget() const;

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	void ClearTarget();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<class UBehaviorTree> m_pBehaviorTree;


	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fSearchRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fSearchCooldown = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fMaxAggroRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	bool m_bUseMaxAggroRange = false;

private:
	// Target
	TWeakObjectPtr<AActor> m_pCachedTarget;
	float m_fSearchCooldownTimer = 0.f;
	bool IsTargetValid(AActor* Target) const;

};
