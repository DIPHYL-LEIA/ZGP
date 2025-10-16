// Fill out your copyright notice in the Description page of Project Settings.


#include "TagSystemComponent.h"
#include "Taggable.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTagSystemComponent::UTagSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentActiveIndex = -1;
}

void UTagSystemComponent::RequestTag()
{
}


// Called when the game starts
void UTagSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeTeam();
	
}

void UTagSystemComponent::InitializeTeam()
{
}


// Called every frame
void UTagSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

