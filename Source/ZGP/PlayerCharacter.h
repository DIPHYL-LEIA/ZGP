// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Taggable.h"
#include "Targetable.h"
#include "SkillExecutor.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChainAttackSkillFinish);

UCLASS()
class ZGP_API APlayerCharacter : public ABaseCharacter, public ITaggable, public ITargetable, public ISkillExecutor
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

	UFUNCTION(BlueprintCallable, Category = "Parry")
	void RequestParryAttack(AActor* ParriedEnemy);

	class USkillComponent* GetSkillComponent() const { return m_pSkillComponent; }

	// Hard Lock Setting (Controller에서 호출)
	void SetHardLockTarget(AActor* Target);
	void ClearHardLockTarget();

	UPROPERTY(BlueprintAssignable, Category = "Chain Attack")
	FOnChainAttackSkillFinish OnChainAttackSkillFinish;

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

	// Skill ID
	UPROPERTY(EditDefaultsOnly)
	FName m_ChainAttackSkillID = FName("ChainAttack");

	UPROPERTY(EditDefaultsOnly)
	FName m_ParryAttackSkillID = FName("ParryAttack");

	UPROPERTY(EditDefaultsOnly)
	float m_fForceTagOutDelay = 3.0f;

	// Chain Attack State
	bool m_bIsChainAttack = false;

	void HandleSkillMontageEnded();

private:
	// 태그 시 남은 행동 처리
	bool m_bPendingTagOut = false;
	FTimerHandle ForceTagOutTimerHandle;

	UFUNCTION()
	void HandleActionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void PerformTagIn(const FVector& TargetLocation, const FRotator& TargetRotation);
	void ExecuteActionTagOut();
	void SetActionTagOutState(bool bActive);

	AActor* GetCurrentTarget() const;
	bool IsHardLock() const;
};
