// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIState.generated.h"

UENUM(BlueprintType)
enum class EEnemyAIState : uint8
{
	IDLE		UMETA(DisplayName = "Idle"),
	CHASE		UMETA(DisplayName = "Chase"),
	ATTACK		UMETA(DisplayName = "Attack"),
	HIT			UMETA(DisplayName = "Hit"),
	DAZED		UMETA(DisplayName = "Dazed"),
	DEAD		UMETA(DisplayName = "Dead"),
};