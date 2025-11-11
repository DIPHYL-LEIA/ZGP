// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestPlayMontage, UAnimMontage*, MontageToPlay);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	virtual void InitializeComponent() override;

	// Bind ComboComponent
	UFUNCTION()
	void ExecuteComboAttack(int32 ComboStep);

	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnRequestPlayMontage OnRequestPlayMontage;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDataTable> m_pSkillDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> m_pComboDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	FName m_currentComboID;

};
