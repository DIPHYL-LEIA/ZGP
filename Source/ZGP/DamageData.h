// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeType.h"
#include "DamageData.generated.h"

UENUM(BlueprintType)
enum class EHitReactionType :uint8
{
	NONE		UMETA(DisplayName = "None"),
	LIGHT		UMETA(DisplayName = "Light"),
	MEDIUM		UMETA(DisplayName = "Medium"),
	HEAVY		UMETA(DisplayName = "Heavy"),
	LAUNCH		UMETA(DisplayName = "Launch"),
	KNOCKDOWN	UMETA(DisplayName = "Knockdown")
};

USTRUCT(BlueprintType)
struct FDamageData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float BaseDamageValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DazeValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EAttributeType AnomalyType = EAttributeType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float AnomalyValue = 0.f;

	// 공격자 정보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TWeakObjectPtr<AActor> Attacker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	EHitReactionType HitReaction = EHitReactionType::LIGHT;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FVector HitDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	FVector HitLocation = FVector::ZeroVector;



};