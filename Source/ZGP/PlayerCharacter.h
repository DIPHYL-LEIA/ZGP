// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Taggable.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class ZGP_API APlayerCharacter : public ABaseCharacter, public ITaggable
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void OnTagIn_Implementation() override;
	virtual void OnTagOut_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> FollowCamera;


};
