// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ZGPPlayerState.generated.h"

class UTagSystemComponent;
/**
 * 
 */
UCLASS()
class ZGP_API AZGPPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AZGPPlayerState();

	UTagSystemComponent* GetTagSystemComponent() const { return TagSystemComponent; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UTagSystemComponent> TagSystemComponent;
};
