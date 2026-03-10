// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ActionState.h"
#include "SkillData.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	NONE				UMETA(DisplayName = "None"),

	// Basic Attack
	BASIC_ATTACK		UMETA(DisplayName = "Basic Attack"),
	
	// Special
	SPECIAL				UMETA(DisplayName = "Special"),
	EX_SPECIAL			UMETA(DisplayName = "Ex Special"),
	
	// Ultimate
	ULTIMATE			UMETA(DisplayName = "Ultimate"),

	// Dodge
	DODGE				UMETA(DisplayName = "Dodge"),
	DODGE_COUNTER		UMETA(DisplayName = "Dodge Counter"),
	DASH_ATTACK			UMETA(DisplayName = "Dash Attack"),

	// Assist
	ASSIST_FOLLOWUP		UMETA(DisplayName = "Assist Follow UP"),
	QUICK_ASSIST		UMETA(DisplayName = "Quick Assist"),

	// Chain Attack
	CHAIN_ATTACK		UMETA(DisplayName = "Chain Attack"),
	PARRY_COUNTER		UMETA(DisplayName = "Parry Counter"),

	MAX					UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Basic Info
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	ESkillType SkillType = ESkillType::BASIC_ATTACK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	TSoftObjectPtr<UAnimMontage> Montage;

	// Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	float DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	float DazeValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	float AnomalyValue = 0.0f;

	// Heavy Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsHeavyAttack = false;

	// Resource Cost
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float EnergyCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float DecibelCost = 0.0f;

	// Resource 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float ResourceGain = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float DecibelGain = 0.f;

	// Cancel
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancel")
	bool bCanCancelSpecial = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancel")
	bool bCanCancelDodge = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cancel")
	bool bCanCancelUltimate = true;
};
