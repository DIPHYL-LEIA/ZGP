// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyType.generated.h"


UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	NORMAL		UMETA(DisplayName = "Normal"),
	ELITE		UMETA(DisplayName = "Elite"),
	BOSS		UMETA(DisplayName = "Boss")
};
