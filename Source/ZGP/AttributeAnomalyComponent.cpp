// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeAnomalyComponent.h"

// Sets default values for this component's properties
UAttributeAnomalyComponent::UAttributeAnomalyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAttributeAnomalyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UAttributeAnomalyComponent::TakeAnomalyDamage(float DamageAmount, EAttributeType Type)
{
}
