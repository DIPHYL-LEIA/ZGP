// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPPlayerState.h"
#include "TagSystemComponent.h"

AZGPPlayerState::AZGPPlayerState()
{
	TagSystemComponent = CreateDefaultSubobject<UTagSystemComponent>(TEXT("TagSystemComponent"));
}
