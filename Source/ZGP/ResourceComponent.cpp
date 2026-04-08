// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceComponent.h"

UResourceComponent::UResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	m_fCurrentEnergy = 0.f;
	m_fCurrentDecibel = 0.f;
}

void UResourceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	m_fCurrentEnergy = 0.f;
	m_fCurrentDecibel = 0.f;
}

bool UResourceComponent::HasEnoughEnergy(float Cost) const
{
	if (Cost <= 0.f) return true;
	return m_fCurrentEnergy >= Cost;
}

bool UResourceComponent::HasEnoughDecibel(float Cost) const
{
	if (Cost <= 0.f) return true;
	return m_fCurrentDecibel >= Cost;
}

bool UResourceComponent::ConsumeEnergy(float Amount)
{
	if (Amount <= 0.f) return true;
	if (m_fCurrentEnergy < Amount) return false;

	m_fCurrentEnergy = FMath::Max(0.f, m_fCurrentEnergy - Amount);
	OnEnergyChanged.Broadcast(m_fCurrentEnergy, m_fMaxEnergy);

	return true;
}

bool UResourceComponent::ConsumeDecibel(float Amount)
{
	if (Amount <= 0.f) return true;
	if (m_fCurrentDecibel < Amount) return false;

	m_fCurrentDecibel = FMath::Max(0.f, m_fCurrentDecibel - Amount);
	OnDecibelChanged.Broadcast(m_fCurrentDecibel, m_fMaxDecibel);

	return true;
}

void UResourceComponent::AddEnergy(float Amount)
{
	if (Amount <= 0.f) return;

	m_fCurrentEnergy = FMath::Clamp(m_fCurrentEnergy + Amount, 0.f, m_fMaxEnergy);
	OnEnergyChanged.Broadcast(m_fCurrentEnergy, m_fMaxEnergy);

	UE_LOG(LogTemp, Warning, TEXT("[Resource] Energy: %.1f / %.1f (+%.1f)"),
		m_fCurrentEnergy, m_fMaxEnergy, Amount);
}

void UResourceComponent::AddDecibel(float Amount)
{
	if (Amount <= 0.f) return;

	m_fCurrentDecibel = FMath::Clamp(m_fCurrentDecibel + Amount, 0.f, m_fMaxDecibel);
	OnDecibelChanged.Broadcast(m_fCurrentDecibel, m_fMaxDecibel);

	UE_LOG(LogTemp, Warning, TEXT("[Resource] Decibel: %.1f / %.1f (+%.1f)"),
		m_fCurrentDecibel, m_fMaxDecibel, Amount);
}

void UResourceComponent::ResetDecibel()
{
	m_fCurrentDecibel = 0.f;
	OnDecibelChanged.Broadcast(m_fCurrentDecibel, m_fMaxDecibel);
}
