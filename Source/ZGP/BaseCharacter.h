// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActionStateProvider.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ZGP_API ABaseCharacter : public ACharacter, public IActionStateProvider
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual bool CanChangeActionState(EActionState NewState) const override;
	virtual void SetActionState(EActionState NewState) const override;
	virtual bool IsActionState(EActionState State) const override;

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// 컴포넌트 추가
	/// </summary>
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UActionStateComponent> m_pActionStateComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkillComponent> m_pSkillComponent;

	UFUNCTION()
	void HandlePlayMontage(UAnimMontage* MontagePlay);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UActionStateComponent* GetActionStateComponent() const;
};
