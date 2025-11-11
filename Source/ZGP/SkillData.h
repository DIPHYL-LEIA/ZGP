// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ActionState.h"
#include "SkillData.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	BASIC_ATTACK	UMETA(DisplayName = "Basic Attack"),
	SPECIAL_SKILL	UMETA(DisplayName = "Special Skill"),
	DODGE			UMETA(DisplayName = "Dodge")
};

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillDate")
	ESkillType SkillType = ESkillType::BASIC_ATTACK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillDate")
	TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillDate")
	float DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillDate")
	float DazeValue = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillDate")
	float AnomalyValue = 0.0f;
};
