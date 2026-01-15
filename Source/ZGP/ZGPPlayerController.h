// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TargetProvider.h"
#include "SquadAction.h"
#include "ZGPPlayerController.generated.h"


UCLASS()
class ZGP_API AZGPPlayerController : public APlayerController, public ITargetProvider, public ISquadAction
{
	GENERATED_BODY()

public:
	AZGPPlayerController();

	virtual void SetupInputComponent() override;

	virtual AActor* GetCurrentTargetActor_Implementation() override;
	virtual void SetCurrentTargetActor_Implementation(AActor* NewTarget) override;
	virtual bool IsTargetLock_Implementation() const override;

	virtual bool TryTriggerChainAttack_Implementation(AActor* DazedEnemy) override;
	virtual bool TryTriggerParryAssist_Implementation(AActor* Attacker, const FVector& AttackDirection) override;
	virtual bool TryTriggerQuickAssist_Implementation() override;

	// UI
	UFUNCTION(BlueprintCallable, Category = "ChainAttack")
	void SelectChainAttackCharacter(int32 SelectedIndex);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION()
	void HandleChainAttackSkillFinish();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USquadManagerComponent> m_pSquadManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UTargetingComponent> m_pTargetingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UParryDetectorComponent> m_pParryDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UChainAttackComponent> m_pChainAttackComponent;
	

	// Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Tag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dodge;

protected:
	// (선택)입력 매핑 우선 순위 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DefaultInputMappingPriority = 0;

	void HandleMove(const struct FInputActionValue& Value);
	void HandleStopMove();
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleStopJumping();
	void HandleDodge();

	UFUNCTION()
	void HandleParryTag(APawn* NewActiveCharacter, AActor* ParriedEnemy);

	// Chain Attack Handler
	UFUNCTION()
	void HandleChainAttackExecute();
	UFUNCTION()
	void HandleChainAttackCancel();
	UFUNCTION()
	void HandleChainAttackFinish();

	void HandleAttack();
	void HandleTag();
	void HandleLockOn();

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> m_pCurrentChainTarget;

	void PauseDazeTimer(AActor* Target, bool bPause);

};
