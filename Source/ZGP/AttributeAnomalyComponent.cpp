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

void UAttributeAnomalyComponent::TakeAnomalyDamage(float DamageValue, EAttributeType Type)
{
	if (Type == EAttributeType::NONE || DamageValue == 0.0f) return;

	float& CurrentGauge = m_mapAnomalyGauges.FindOrAdd(Type);
	CurrentGauge = FMath::Clamp(CurrentGauge + DamageValue, 0.f, m_fMaxAnomalyGauges);

	if (CurrentGauge >= m_fMaxAnomalyGauges)
	{
		// 이미 터진 상태인지 (버프 컴포넌트 확인 필요)

		OnAnomalyStateApplied.Broadcast(Type);

		CurrentGauge = 0.f;

		UE_LOG(LogTemp, Log, TEXT("[AttributeAnomalyComponent] TakeAnomalyDamage : Anomaly Triggered %d"), (int32)Type);
	}
}
