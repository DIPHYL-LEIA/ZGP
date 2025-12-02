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

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	AActor* GetCurrentTarget();

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	AActor* FindNewTarget() const;

	UFUNCTION(BlueprintCallable, Category = "AI Targeting")
	void ClearTarget();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fSearchRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fSearchCooldown = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	float m_fMaxAggroRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Targeting")
	bool m_bUseMaxAggroRange = false;

private:
	TWeakObjectPtr<AActor> CachedTarget;
	float m_fSearchCooldownTimer = 0.f;

	bool IsTargetValid(AActor* Target) const;

};
