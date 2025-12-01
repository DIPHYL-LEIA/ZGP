// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPerceptionComponent.h"

UEnemyPerceptionComponent::UEnemyPerceptionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UEnemyPerceptionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

