// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../DamageData.h"
#include "EnemyAttackData.generated.h"

/// <summary>
/// 적 공격 타입
/// </summary>

UENUM(BlueprintType)
enum class EEnemyAttackType : uint8
{
	NONE			UMETA(DisplayName = "None"),
	
	// 근접 공격
	MELEE_LIGHT		UMETA(DisplayName = "Melee Light"),		// 약공격
	MELEE_HEAVY		UMETA(DisplayName = "Melee Heavy"),		// 강공격
	MELEE_COMBO		UMETA(DisplayName = "Melee Combo"),		// 콤보 공격

	// 원거리 공격
	RANGE_SINGLE	UMETA(DisplayName = "Range Single"),		// 단발
	RANGE_BURST		UMETA(DisplayName = "Range Burst"),		// 연사

	CHARGE			UMETA(DisplayName = "Charge"),
	RUSH			UMETA(DisplayName = "Rush"),
	AOE				UMETA(DisplayName = "AoE"),
	GRAB			UMETA(DisplayName = "Grab"),

	// 보스 전용
	SPECIAL			UMETA(DisplayName = "Special"),
	ULTIMATE		UMETA(DisplayName = "Ultimate"),

	MAX				UMETA(DisplayName = "Max")
};


// 공격 거리
UENUM(BlueprintType)
enum class EAttackRangeZone : uint8
{
	CLOSE		UMETA(DisplayName = "Close"),
	MID			UMETA(DisplayName = "Mid"),
	LONG		UMETA(DisplayName = "Long"),
	ANY			UMETA(DisplayName = "Any"),
	MAX			UMETA(DisplayName = "Max")
};

/// <summary>
/// 플래시 타입
/// 
/// Gold	: 패리/회피 가능
/// Red		: 회피만 가능
/// None	: 플래시 없음
/// </summary>
UENUM(BlueprintType)
enum class EAttackFlashType : uint8
{
	NONE		UMETA(DisplayName = "None"),
	GOLD		UMETA(DisplayName = "Gold"),
	RED			UMETA(DisplayName = "Red"),
	MAX			UMETA(DisplayName = "Max"),
};


/// <summary>
/// 적 공격 데이터
/// </summary>
USTRUCT(BlueprintType)
struct FEnemyAttackData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	EEnemyAttackType AttackType = EEnemyAttackType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	TSoftObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FName MontageSection = NAME_None;
	
	// 공격 설명(디버그 / UI용) 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Basic")
	FText Description;

	/* 공격 거리 조건 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range", meta = (ClampMin = "0.0"))
	float MinRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range", meta = (ClampMin = "0.0"))
	float MaxRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
	EAttackRangeZone RangeZone = EAttackRangeZone::CLOSE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parry")
	EAttackFlashType FlashType = EAttackFlashType::NONE;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parry", meta = (ClampMin = "1.0"))
	float ParryDazeBonus = 2.0f;

	/// <summary>
	/// 패리 가능 여부
	/// GOLD	: true
	/// RED		: false
	/// </summary>
	bool IsParryable() const { return FlashType == EAttackFlashType::GOLD; }
	bool IsUnblockable() const { return FlashType == EAttackFlashType::RED; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float DazeValue = 0.f;			// 플레이어에게 사용 안 함(확장용)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EHitReactionType HitReaction = EHitReactionType::LIGHT;

	// 랜덤 선택 시 가중치(높을수록 자주 선택됨)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection", meta = (ClampMin = "0.0"))
	float SelectWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection", meta = (ClampMin = "0.0"))
	float Cooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection")
	int32 Priority = 0;

	// 콤보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	FName NextComboID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ComboChainChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bIsComboStart = false;

	/* 필요 HP 비율일 때만 사용 가능 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RequiredHPRatioUnder = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float RequiredHPRatioUp = 0.f;

	/* 특정 페이즈에서 사용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	int32 RequiredPhase = 0;

	bool IsInRange(float Distance) const
	{
		return Distance >= MinRange && Distance <= MaxRange;
	}

	bool IsHPCondition(float CurrentHPRatio) const
	{
		if (RequiredHPRatioUnder > 0.f && CurrentHPRatio > RequiredHPRatioUnder)
			return false;
		if (RequiredHPRatioUp > 0.f && CurrentHPRatio < RequiredHPRatioUp)
			return false;

		return true;
	}

	bool IsPhaseCondition(int32 CurrentPhase) const
	{
		return RequiredPhase == 0 || RequiredPhase == CurrentPhase;
	}
};


/*
* 적 공격 패턴
* 몬스터 타입별로 사용 가능한 공격
*/

USTRUCT(BlueprintType)
struct FEnemyAttackPattern : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
	FText PatternName;

	// 근거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Distance Attack")
	TArray<FName> CloseRangeAttack;

	// 중거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Distance Attack")
	TArray<FName> MidRangeAttack;

	// 원거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Distance Attack")
	TArray<FName> LongRangeAttack;

	// 반격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Attack")
	TArray<FName> CounterAttack;

	// HP 낮을 때
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Attack")
	TArray<FName> EnrageAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Special Attack")
	TArray<FName> OpenAttack;

	/* 보스 전용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase Attack")
	TArray<FName> Phase1Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase Attack")
	TArray<FName> Phase2Attack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase Attack")
	TArray<FName> Phase3Attack;

	const TArray<FName>& GetAttackByDistance(float Distance) const
	{
		if (Distance <= 150.f)
		{
			return CloseRangeAttack;
		}
		else if (Distance <= 400.f)
		{
			return MidRangeAttack;
		}
		return LongRangeAttack;
	}

	const TArray<FName>& GetAttackPhase(int32 Phase) const
	{
		switch (Phase)
		{
		case 1:
			return Phase1Attack;
		case 2:
			return Phase2Attack;
		case 3:
			return Phase3Attack;
		default:
			return Phase1Attack;
		}
	}

	static int32 CalculatePhase(float HPRatio)
	{
		if (HPRatio > 0.7f) 
			return 1;
		if (HPRatio > 0.3f)
			return 2;
		return 3;
	}
};

