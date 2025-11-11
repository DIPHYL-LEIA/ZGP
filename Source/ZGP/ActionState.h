// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionState.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	IDLE		UMETA(DisplayName = "Idle"),
	WALKING		UMETA(DisplayName = "Walking"),
	RUNNING		UMETA(DisplayName = "Running"),
	ATTACKING	UMETA(DisplayName = "Attacking"),
	DODGING		UMETA(DisplayName = "Dodging"),
	HIT			UMETA(DisplayName = "Hit"),
	DAZED		UMETA(DisplayName = "Dazed"),
	DEAD		UMETA(DisplayName = "Dead"),
	MAX			UMETA(Hidden)
};
