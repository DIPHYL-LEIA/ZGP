// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeType.h"
#include "DamageData.generated.h"

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

};