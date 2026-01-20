// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlashComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


UEnemyFlashComponent::UEnemyFlashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyFlashComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UEnemyFlashComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void UEnemyFlashComponent::TriggerFlash(EAttackFlashType FlashType, float Duration)
{
}

void UEnemyFlashComponent::EndFlash()
{
}

void UEnemyFlashComponent::StartNiagaraEffect(EAttackFlashType FlashType)
{
}

void UEnemyFlashComponent::StopNiagaraEffect()
{
}

void UEnemyFlashComponent::PlayFlashSound(EAttackFlashType FlashType)
{
}
