// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FBTContext
{
	class AAIController* Controller = nullptr;
	class APawn* Pawn = nullptr;
	class UBlackboardComponent* BB = nullptr;
	class AActor* Target = nullptr;

	bool HasTarget() const { return Target != nullptr; }
};
