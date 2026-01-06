// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarWidget.generated.h"

/**
 * 체력/Daze 등의 바 형태 UI
 */
UCLASS()
class ZGP_API UBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBarWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetPercent(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetPercentNow(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetUseRealTime(bool bUseRealTime);

protected:


};
