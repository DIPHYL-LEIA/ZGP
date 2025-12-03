// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPGameState.h"
#include "TokenManagerComponent.h"

AZGPGameState::AZGPGameState()
{
	m_pTokenManagerComponent = CreateDefaultSubobject<UTokenManagerComponent>(TEXT("TokenManagerComponent"));
}

UTokenManagerComponent* AZGPGameState::GetTokenComponent() const
{
	return m_pTokenManagerComponent;
}
