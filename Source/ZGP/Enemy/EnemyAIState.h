// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIState.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	NONE			UMETA(DisplayName = "None"),
	CHASE			UMETA(DisplayName = "Chase"),
	// 전투 범위 내 대기
	COMBAT_IDLE		UMETA(DisplayName = "Combat Idle"),
	// 측면 이동
	STRAFE			UMETA(DisplayName = "Strafe"),
	// 후퇴
	RETREAT			UMETA(DisplayName = "Retreat")
};