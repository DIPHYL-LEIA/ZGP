// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ComboData.generated.h"

/// <summary>
/// Input Type
/// </summary>

UENUM(BlueprintType)
enum class EInputAttackType : uint8
{
	TAP		UMETA(DisplayName = "Tap"),
	HOLD	UMETA(DisplayName = "Hold"),
	DELAY	UMETA(DisplayName = "Delay")
};

/// <summary>
/// Combo Node 구조체
/// 
/// 하나의 콤보 스텝에 대한 정보
/// </summary>
USTRUCT(BlueprintType)
struct FComboNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsLastCombo = false;

	// Tap / Hold
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputAttackType InputType = EInputAttackType::TAP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NextComboIndex_Tap = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NextComboIndex_Hold = -1;

	// Hold 판정에 걸리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HoldTime = 0.5; 

	// Hold 공격 있는지(없으면 Tap만)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsHold = false;

};

USTRUCT(BlueprintType)
struct FComboData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	TArray<FComboNode> ComboChain;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	float ComboResetTime = 1.5f;

	const FComboNode* GetComboNode(int32 Index) const
	{
		if (ComboChain.IsValidIndex(Index))
		{
			return &ComboChain[Index];
		}
		return nullptr;
	}

	int32 GetNextComboIndex(int32 CurrentIndex, EInputAttackType InputType) const
	{
		const FComboNode* Node = GetComboNode(CurrentIndex);
		if (!Node) return -1;

		// 콤보 분기 비활성화 & Hold 아니면 Tap 처리
		if (!Node->bIsHold || InputType != EInputAttackType::HOLD)
		{
			return Node->NextComboIndex_Tap;
		}

		// Hold가 -1이면 Tap 처리
		if (Node->NextComboIndex_Hold == -1)
		{
			return Node->NextComboIndex_Tap;
		}
		return Node->NextComboIndex_Hold;
	}

	bool IsEmpty() const
	{
		return ComboChain.Num() == 0;
	}

	// 콤보 체인 길이
	int32 GetComboLength() const
	{
		return ComboChain.Num();
	}
};