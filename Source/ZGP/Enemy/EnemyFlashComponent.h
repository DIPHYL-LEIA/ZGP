// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackData.h"
#include "EnemyFlashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttackFlashTrigger, AActor*, Attacker, EAttackFlashType, FlashType, bool, bIsParryable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFlashEnd, AActor*, Attacker);

/*
* 패리 윈도우 관리는 AnimNotifyState_ParryWindow가 담당
* 이 컴포넌트는 시각적 피드백만 담당
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFlashComponent();

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void TriggerFlash(EAttackFlashType FlashType, float Duration = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void EndFlash();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};