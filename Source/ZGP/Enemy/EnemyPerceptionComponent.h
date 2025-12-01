// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyPerceptionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyPerceptionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
//	// Spawn 되자마자 Targeting 지정
//	void InitializeSensor();
//
//	UFUNCTION(BlueprintPure, Category = "Perception")
//	AActor* GetCurrentTarget() const;
//
//	UFUNCTION(BlueprintPure, Category = "Perception")
//	float GetDistanceTarget() const;
//		
//	UFUNCTION(BlueprintPure, Category = "Perception")
//	bool IsInRange(AActor* TargetActor) const;
//
//
//public:
//
//protected:
//	void UpdatePerception();
//	AActor* FindBestTarget();
//	float CalculateTargetScore(AActor* TargetActor, const FVector& Location, const FVector& Forward);
//	bool CheckSight(AActor* TargetActor) const;

protected:
	TWeakObjectPtr<AActor> m_pCurrentTarget;
	FTimerHandle m_sensorTimerHandle;

};
