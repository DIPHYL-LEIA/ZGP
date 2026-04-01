// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Taggable.h"
#include "Targetable.h"
#include "SkillExecutor.h"
#include "ResourceProvider.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChainAttackSkillFinish);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerPerfectDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAttackStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHitReactionStart, EHitReactionType, ReactionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerHitReactionEnd);

UCLASS()
class ZGP_API APlayerCharacter : public ABaseCharacter, public ITaggable, public ITargetable, public ISkillExecutor, public IResourceProvider
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	void Move(const struct FInputActionValue& Value);
	void StopMove();
	void Look(const FInputActionValue& Value);
	void RequestAttack();
	void RequestDodge();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Tag
	virtual bool CanTag_Implementation() const override;
	virtual void OnTagIn_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;
	virtual void OnTagOut_Implementation() override;
	virtual void OnTagOutAction_Implementation() override;
	virtual void OnChainAttackTag_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation, AActor* TargetEnemy) override;

	// Target
	virtual bool IsTargetable_Implementation() const override;
	virtual FVector GetTargetLocation_Implementation() const override;
	virtual void OnTargeted_Implementation(bool IsTargeted) override;
	virtual void OnUnTargeted_Implementation() override;

	// Skill
	virtual bool ExecuteSkillByID_Implementation(FName SkillID) override;
	virtual bool IsExecuteSkill_Implementation() const override;
	virtual bool IsCurrentSkillHeavy_Implementation() const override;
	virtual void NotifySkillCompleted_Implementation() override;
	virtual FName GetCurrentSkillID_Implementation() const override;

	// Combat
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;

	// Resource
	virtual bool HasResource_Implementation(float EnergyCost, float DecibelCost) const override;
	virtual void ConsumeResource_Implementation(float EnergyCost, float DecibelCost) override;
	virtual void AddResource_Implementation(float EnergyGain, float DecibelGain) override;

	UFUNCTION(BlueprintCallable)
	void RequestParryAttack(AActor* ParriedEnemy);

	// Reactive Assist
	UFUNCTION(BlueprintCallable, Category = "Assist")
	void ExecuteReactiveAssistSkill(AActor* TargetEnemy);

	UFUNCTION(BlueprintCallable, Category = "Assist")
	void ExecuteQuickAssistSkill(AActor* TargetEnemy);

	// 조회
	EHitReactionType GetCurrentHitReactionType() const { return m_eCurrentHitReactionType; }
	class USkillComponent* GetSkillComponent() const { return m_pSkillComponent; }
	class UResourceComponent* GetResourceComponent() const { return m_pResourceComponent; }

	// Hard Lock Setting (Controller에서 호출)
	void SetHardLockTarget(AActor* Target);
	void ClearHardLockTarget();

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnChainAttackSkillFinish OnChainAttackSkillFinish;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnPlayerPerfectDodge OnPlayerPerfectDodge;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnPlayerAttackStart  OnPlayerAttackStart;

	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnPlayerHitReactionStart OnPlayerHitReactionStart;
	
	UPROPERTY(BlueprintAssignable, Category = "Combat")
	FOnPlayerHitReactionEnd OnPlayerHitReactionEnd;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> m_SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> m_Camera;


	// Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillComponent> m_pSkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UComboComponent> m_pComboComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDodgeComponent> m_pDodgeCompComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPlayerLocoComponent> m_pPlayerLocoComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UPlayerCameraComponent> m_pPlayerCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UResourceComponent> m_pResourceComponent;

	// Skill ID
	UPROPERTY(EditDefaultsOnly)
	FName m_ChainAttackSkillID = FName("ChainAttack");

	UPROPERTY(EditDefaultsOnly)
	FName m_ParryAttackSkillID = FName("ParryAttack");

	UPROPERTY(EditDefaultsOnly)
	FName m_ReactiveAssistSkillID;

	UPROPERTY(EditDefaultsOnly)
	FName m_QuickAssistSkillID;

	// Chain Attack State
	bool m_bIsChainAttack = false;

	UPROPERTY(EditDefaultsOnly)
	float m_fForceTagOutDelay = 3.0f;

	UPROPERTY(EditDefaultsOnly)
	EHitReactionType m_eCurrentHitReactionType = EHitReactionType::NONE;

	void HandleSkillMontageEnded();

	virtual void HandleHitReactionStart(EHitReactionType ReactionType, bool bCancel) override;
	virtual void HandleHitReactionEnd() override;

private:
	// 태그 시 남은 행동 처리
	bool m_bPendingTagOut = false;
	FTimerHandle ForceTagOutTimerHandle;

	UFUNCTION()
	void HandlePerfectDodge();

	UFUNCTION()
	void HandleActionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PerformTagIn(const FVector& TargetLocation, const FRotator& TargetRotation);
	void ExecuteActionTagOut();
	void SetActionTagOutState(bool bActive);

	AActor* GetCurrentTarget() const;
	bool IsHardLock() const;
};
