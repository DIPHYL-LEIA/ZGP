// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ComboData.generated.h"

UENUM(BlueprintType)
enum class EInputAttackType : uint8
{
	TAP		UMETA(DisplayName = "Tap"),
	HOLD	UMETA(DisplayName = "Hold"),
	DELAY	UMETA(DisplayName = "Delay Input")
};

USTRUCT(BlueprintType)
struct FComboNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputAttackType InputType = EInputAttackType::TAP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NextComboID_Tap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName NextComboID_Hold;

};

USTRUCT(BlueprintType)
struct FComboData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TMap<int32, FComboNode> ComboNodes;
};