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

	void Move(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> m_SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> m_Camera;


};
