// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeType.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	NONE		UMETA(DisplayName = "None"),
	PHYSICAL	UMETA(DisplayName = "Physical"),
	FIRE		UMETA(DisplayName = "Fire"),
	ICE			UMETA(DisplayName = "Ice"),
	ELECTRIC	UMETA(DisplayName = "Electric"),
	ETHER		UMETA(DisplayName = "Ether"),
	MAX
};