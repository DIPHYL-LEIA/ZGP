// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIState.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIStae : uint8
{
	IDLE		UMETA(DisplayName = "Idle"),
	CHASE		UMETA(DisplayName = "Chase"),
	ATTACK		UMETA(DisplayName = "Attack"),
	STOP		UMETA(DisplayName = "Stop"),
	DEAD		UMETA(DisplayName = "Dead"),
};