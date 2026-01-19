// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlashComponent.h"

UEnemyFlashComponent::UEnemyFlashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyFlashComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEnemyFlashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

