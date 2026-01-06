// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../AttributeType.h"
#include "UIType.generated.h"

// UI 상태
UENUM(BlueprintType)
enum class E_UI_STATE : uint8
{
	NONE			UMETA(DisplayName = "None"),
	COMBAT			UMETA(DisplayName = "Combat"),
	EXPLORATION		UMETA(DisplayName = "Exploration"),
	MENU			UMETA(DisplayName = "Menu"),
	DIALOGUE		UMETA(DisplayName = "Dialoue"),
	CHAIN_ATTACK	UMETA(DisplayName = "Chain Attack")
};

// 적 UI 표시
UENUM(BlueprintType)
enum class E_UI_ENEMY : uint8
{
	NONE			UMETA(DisplayName = "None"),
	WORLD			UMETA(DisplayName = "World"),
	HUD				UMETA(DisplayName = "HUD"),
	BOTH			UMETA(DisplayName = "Both")
};

// 게이지 바	(게이지 애니메이션)
UENUM(BlueprintType)
enum class E_UI_GAUGEBAR : uint8
{
	IDLE		UMETA(DisplayName = "Idle"),
	INCREASE	UMETA(DisplayName = "Increase"),
	DECREASE	UMETA(DisplayName = "Decrease")
};

// 공격에 따른 대미지 디자인
UENUM(BlueprintType)
enum class E_UI_DAMAGE_DESIGN : uint8
{
	ORIGINAL	UMETA(DisplayName = "Original"),
	CRITICAL	UMETA(DisplayName = "Critical"),
	ATTRIBUTE	UMETA(DisplayName = "Attribute"),
	DAZE		UMETA(DisplayName = "Daze"),
	HEAL		UMETA(DisplayName = "Heal"),
	IMMUNE		UMETA(DisplayName = "Immune")
};

// 스쿼드 상태
UENUM(BlueprintType)
enum class E_UI_SQUAD_SLOT_STATE : uint8
{
	INACTIVE			UMETA(DisplayName = "Inactive"),
	ACTIVE				UMETA(DisplayName = "Active"),
	QUICK_ASSIST_READY	UMETA(DisplayName = "QuickAssistReady"),
	DEAD				UMETA(DisplayName = "Dead")
};

// 타겟팅 마커
UENUM(BlueprintType)
enum class E_UI_LOCKON : uint8
{
	NONE		UMETA(DisplayName = "None"),
	SOFTLOCK	UMETA(DisplayName = "SoftLock"),
	HARDLOCK	UMETA(DisplayName = "HardLock")
};

USTRUCT(BlueprintType)
struct FAttributeUIData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttributeType AttributeType = EAttributeType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor FirstColor = FLinearColor::White;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor SecondColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UTexture2D> Icon;
};
