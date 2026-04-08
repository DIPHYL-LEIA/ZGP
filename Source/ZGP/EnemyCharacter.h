// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Targetable.h"
#include "Dazeable.h"
#include "SkillExecutor.h"
#include "Enemy/EnemyType.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ZGP_API AEnemyCharacter : public ABaseCharacter, public ITargetable, public IDazeable, public ISkillExecutor	
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyDazeComponent> m_pEnemyDazeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyUIComponent> m_pEnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyAttackSelectorComponent> m_pEnemyAttackSelectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemySkillComponent> m_pEnemySkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyFlashComponent> m_pEnemyFlashComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	EEnemyType m_eEnemyType = EEnemyType::NORMAL;

	bool m_bParryWindowActive = false;

public:
	AEnemyCharacter();

	UFUNCTION(BlueprintPure)
	EEnemyType GetEnemyType() const { return m_eEnemyType; }

	UEnemyDazeComponent* GetDazeComponent() const;
	class UEnemyUIComponent* GetUIComponent() const;

	// Target
	virtual bool IsTargetable_Implementation() const override;
	virtual FVector GetTargetLocation_Implementation() const override;
	virtual void OnTargeted_Implementation(bool IsTargeted) override;
	virtual void OnUnTargeted_Implementation() override;

	// Daze
	virtual void PauseDazeTimer_Implementation(bool bPause) override;
	virtual bool IsDazed_Implementation() const override;

	// Skill
	virtual bool ExecuteSkillByID_Implementation(FName SkillID) override;
	virtual bool IsExecuteSkill_Implementation() const override;
	virtual bool IsCurrentSkillHeavy_Implementation() const override;
	virtual void NotifySkillCompleted_Implementation() override;
	virtual FName GetCurrentSkillID_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;
	virtual void SetParryWindowActive_Implementation(bool bActive) override;
	virtual bool CanParry_Implementation() const override;

	UFUNCTION()
	void BindUIComponent();

	UFUNCTION()
	void HandleDaze();

	UFUNCTION()
	void HandleDazeRecovered();

	UFUNCTION()
	void HandleHealthStateChanged(AActor* Character, float NewHealth, float MaxHealth);
	
	UFUNCTION()
	void HandleDazeValueChanged(float CurrentDaze, float MaxDaze);

	UFUNCTION()
	void HandleDied(AActor* Character);

	virtual void OnMontageEndedAction(UAnimMontage* Montage, bool bInterrupted) override;

};
