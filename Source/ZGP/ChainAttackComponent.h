// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChainAttackComponent.generated.h"

UENUM(BlueprintType)
enum class EChainAttackState : uint8
{
	INACTIVE		UMETA(DisplayName = "Inactive"),
	WAITING_INPUT	UMETA(DisplayName = "Waiting Input"),
	EXECUTE			UMETA(DisplayName = "Execute")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChainAttackStateChanged, EChainAttackState, OldState, EChainAttackState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChainAttackReady, AActor*, TargetEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChainAttackExecuted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChainAttackCancelled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChainAttackFinished);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UChainAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChainAttackComponent();

	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	bool TriggerChainAttack(AActor* TargetEnemy);

	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	bool ExecuteChainAttack();

	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	void CancelChainAttack();

	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	void FinishChainAttack();

	// 상태 
	UFUNCTION(BlueprintPure, Category = "ChainAttack")
	bool IsChainWaitingInput() const;

	UFUNCTION(BlueprintPure, Category = "ChainAttack")
	bool IsChainExecuteing() const;

	UFUNCTION(BlueprintPure, Category = "ChainAttack")
	bool IsChainActive() const;

	UFUNCTION(BlueprintPure, Category = "ChainAttack")
	EChainAttackState GetChainState() const { return m_eChainState; }

	UFUNCTION(BlueprintPure, Category = "ChainAttack")
	AActor* GetChainTarget() const;

	// 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackStateChanged OnChainAttackStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackReady	OnChainAttackReady;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackExecuted OnChainAttackExecuted;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackCancelled OnChainAttackCancelled;

	UPROPERTY(BlueprintAssignable, Category = "ChainAttack")
	FOnChainAttackFinished OnChainAttackFinished;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly, Category = "ChainAttack")
	EChainAttackState m_eChainState = EChainAttackState::INACTIVE;

	UPROPERTY()
	TWeakObjectPtr<AActor> m_pChainTarget;

	// 슬로우 모션
	UPROPERTY(EditDefaultsOnly, Category = "ChainAttack")
	float m_fGlobalTime = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "ChainAttack")
	float m_fTargetTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "ChainAttack")
	float m_fInputTime = 3.0f;

private:
	FTimerHandle m_WindowTimerHandle;
	float m_fOriginalTargetTime = 1.0f;

	void SetChainState(EChainAttackState NewState);

	void StartSlowMotion();
	void EndSlowMotion();

	void StartWindowTimer();
	void ClearWindowTimer();

	UFUNCTION()
	void HandleWindowTimeOut();

	void StartCameraEffect();
	void EndCameraEffect();

	AActor* GetPlayerPawn() const;
};
