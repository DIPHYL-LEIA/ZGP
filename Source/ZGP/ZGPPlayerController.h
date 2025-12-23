// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TargetProvider.h"
#include "ZGPPlayerController.generated.h"


UCLASS()
class ZGP_API AZGPPlayerController : public APlayerController, public ITargetProvider
{
	GENERATED_BODY()

public:
	AZGPPlayerController();

	virtual void SetupInputComponent() override;

	virtual AActor* GetCurrentTargetActor_Implementation() override;
	virtual void SetCurrentTargetActor_Implementation(AActor* NewTarget) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USquadManagerComponent> m_pSquadManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UTargetingComponent> m_pTargetingComponent;

	// Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Default = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Tag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dodge;

	// Tag
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	TArray<TSubclassOf<class APlayerCharacter>> m_arTagCharacter;

protected:
	// (선택)입력 매핑 우선 순위 
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 DefaultInputMappingPriority = 0;

	void HandleMove(const struct FInputActionValue& Value);
	void HandleLook(const FInputActionValue& Value);
	void HandleJump();
	void HandleStopJumping();
	void HandleDodge();

	void HandleAttack();
	void HandleTag();
	void HandleLockOn();


};
