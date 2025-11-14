// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeType.h"
#include "AttributeAnomalyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnomalyStateApplied, EAttributeType, AppliedAnomalyType);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UAttributeAnomalyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeAnomalyComponent();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Anomaly")
	TMap<EAttributeType, float> m_mapAnomalyGauges;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly")
	float m_fMaxAnomalyGauges = 100.f;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Anomaly")
	void TakeAnomalyDamage(float DamageValue, EAttributeType Type);

	UPROPERTY(BlueprintAssignable, Category = "Anomaly")
	FOnAnomalyStateApplied OnAnomalyStateApplied;

};
