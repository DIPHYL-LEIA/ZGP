// Fill out your copyright notice in the Description page of Project Settings.


#include "HitReactionComponent.h"

UHitReactionComponent::UHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitReactionComponent::RunHitReaction(const FDamageData& DamageData)
{
}
