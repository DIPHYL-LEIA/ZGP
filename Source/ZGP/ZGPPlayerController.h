// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZGPPlayerController.generated.h"


UCLASS()
class ZGP_API AZGPPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Default = nullptr;


	// 입력 매핑 우선 순위 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DefaultInputMappingPriority = 0;

public:
	AZGPPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

};
