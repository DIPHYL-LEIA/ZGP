// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ZGPGameState.generated.h"

UCLASS()
class ZGP_API AZGPGameState : public AGameStateBase
{
	GENERATED_BODY()
	

public:
	AZGPGameState();

	UFUNCTION(BlueprintPure)
	class UTokenManagerComponent* GetTokenComponent() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTokenManagerComponent> m_pTokenManagerComponent;

};
