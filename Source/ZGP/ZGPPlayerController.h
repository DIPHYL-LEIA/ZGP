// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZGPPlayerController.generated.h"


UCLASS()
class ZGP_API AZGPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AZGPPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	
protected:
	// (선택)입력 매핑 우선 순위 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DefaultInputMappingPriority = 0;

};
