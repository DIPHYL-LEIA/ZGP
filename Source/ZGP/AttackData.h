// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageData.h"
#include "AttackData.generated.h"

/// <summary>
/// 공격 데이터 구조체
/// </summary>

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
	
	// Resource
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float ResourceGain = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
	float DecibelGain = 0.f;

	// Damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DamageMultiply = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float DazeValue = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bAllowMultiHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	EHitReactionType HitReaction = EHitReactionType::LIGHT;

	/// <summary>
	/// Heavy Attack이 true일 때, Stunned 상태의 적에게 공격 적중 시 Chain Attack 트리거 발동
	/// 
	/// Basic Attack 마지막 타격
	/// Special / Ex Special
	/// Ultimate
	/// Dodge Counter
	/// Assist Follow Up
	/// Chain Attack
	/// Parry Counter
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bIsHeavyAttack = false;

};
