// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraEffectType.generated.h"

UENUM(BlueprintType)
enum class ECameraEffectType : uint8
{
	NONE			UMETA(DisplayName = "None"),
	PERPECT_DODGE	UMETA(DisplayName = "Perfect Dodge"),
	PARRY_ASSIST	UMETA(DisplayName = "Parry Assist"),
	CHAIN_ATTACK	UMETA(DisplayName = "Chain Attack"),
	ULTIMATE		UMETA(DisplayName = "Ultimate"),
	CUSTOM			UMETA(DisplayName = "Custom"),
};

USTRUCT(BlueprintType)
struct FCameraEffectParams
{
	GENERATED_BODY()

	// Slow Motion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlowMotion")
	bool bEnableSlowMotion = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlowMotion", meta = (EditCondition = "bEnableSlowMotion", ClampMin = "0.01", ClampMax = "1.0"))
	float SlowMotionScale = 0.1f;

	// Zoom
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool bEnableZoom = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom", meta = (EditCondition = "bEnableZoom"))
	float ZoomFOVOffset = -15.f;		// 음수 = 줌 인

	// Camera Shake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	bool bEnableShake = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	TSubclassOf<UCameraShakeBase> ShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
	float ShakeScale = 1.0f;

	// 강제 Hard Lock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Focus")
	bool bFocusTarget = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Duration")
	float Duration = 0.5f;

	// 플레이어 예외 (슬로우 모션에서 제외)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SlowMotion")
	bool bExcludePlayer = true;
};

USTRUCT(BlueprintType)
struct FParrySequenceParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float ImpactTimeScale = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float ImpactRealDuration = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	TSubclassOf<UCameraShakeBase> ImpactShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float ShakeScale = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float ImpactZoomFOV = -15.f;
};
