// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackData.generated.h"

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	// Trace Range
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float Range = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float Radius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DazeValue = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bAllowMultiHit = false;
};
