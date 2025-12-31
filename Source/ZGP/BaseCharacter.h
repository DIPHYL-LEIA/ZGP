// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActionStateProvider.h"
#include "CombatInteraction.h"
#include "CombatAction.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ZGP_API ABaseCharacter : public ACharacter, public IActionStateProvider, public ICombatInteraction, public ICombatAction
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// 컴포넌트 추가
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UActionStateComponent> m_pActionStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillComponent> m_pSkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHealthComponent> m_pHealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAttributeAnomalyComponent> m_pAttributeAnomalyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCombatComponent> m_pCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHitReactionComponent> m_pHitReactionComponent;

	UFUNCTION()
	void HandlePlayMontage(UAnimMontage* MontagePlay);

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void HandleHitReactionStart(EHitReactionType ReactionType, bool bCancel);

	UFUNCTION()
	void HandleHitReactionEnd();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ActionStateProvider
	virtual bool CanChangeActionState(EActionState NewState) const override;
	virtual void SetActionState(EActionState NewState) override;
	virtual bool IsActionState(EActionState State) const override;

	// CombatInteraction
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;
	virtual bool CanParry_Implementation() const override;
	virtual void Parried_Implementation(AActor* Character) override;

	// CombatAction
	virtual void BeginAttack_Implementation() override;
	virtual void EndAttack_Implementation() override;

	UActionStateComponent* GetActionStateComponent() const;
	UHealthComponent* GetHealthComponent() const;
	UAttributeAnomalyComponent* GetAttributeAnomalyComponent() const;
};
