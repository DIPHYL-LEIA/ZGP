// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UEnemyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Enemy HUD")
	void BindEnemy(class AEnemyCharacter* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Enemy HUD")
	void UnbindEnemy();

	// 현재 바인딩 된 적
	UFUNCTION(BlueprintCallable, Category = "Enemy HUD")
	AEnemyCharacter* GetBoundEnemy() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBarWidget> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBarWidget> DazeBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DazeValue;

private:
	TWeakObjectPtr<AEnemyCharacter> m_pBoundEnemy;

	TWeakObjectPtr<class UHealthComponent> m_pHealthComponent;
	TWeakObjectPtr<class UEnemyDazeComponent> m_pEnemyDazeComponent;

	UFUNCTION()
	void HandleHealthChanged(AActor* Character, float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleDazeChanged(float CurrentDaze, float MaxDaze);

	void ClearBinding();

};

