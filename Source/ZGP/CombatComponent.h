// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackData.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttackHit, AActor*, Target, const FVector&, HitLocation, const FVector&, HitDirection);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttackTrace(const FAttackData& AttackData);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void BeginAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndAttack();
		
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnAttackHit OnAttackHit;

protected:
	TSet<TWeakObjectPtr<AActor>> m_setHitActors;

	void ExecuteTrace(const FVector& Start, const FVector& End, float Radius, TArray<FHitResult>& OutHits);
	void ApplyDamage(AActor* Target, const FAttackData& AttackData, const FHitResult& HitResult);

	UPROPERTY(EditDefaultsOnly)
	bool m_bShowDebug = false;

};
