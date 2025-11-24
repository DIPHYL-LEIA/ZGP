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


	void Move(const struct FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void RequestAttack();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Tag
	virtual bool CanTag_Implementation() const override;
	virtual void OnTagIn_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;
	virtual void OnTagOut_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> m_SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> m_Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UComboComponent> m_pComboComp;


};
