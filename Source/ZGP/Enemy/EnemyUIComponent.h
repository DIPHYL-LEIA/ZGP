// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyUIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyUIComponent();

	UFUNCTION()
	void OnHealthChanged(AActor* Character, float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void OnDazeChanged(float CurrentDaze, float MaxDaze);

	UFUNCTION()
	void OnDied(AActor* Character);

	UFUNCTION(BlueprintCallable, Category = "EnemyUI")
	void OnTargetStateChanged(bool bIsTargeted, bool bIsHardLock);

	UFUNCTION(BlueprintCallable, Category = "EnemyUI")
	void InitializeValue(float HealthPercent, float DazePercent);

	UFUNCTION(BlueprintCallable, Category = "EnemyUI")
	void ShowEnemyUI(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "EnemyUI")
	bool IsEnemyUIVisible() const { return m_bIsEnemyUIVisible; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyUI")
	TSubclassOf<class UUserWidget> m_BarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyUI")
	FVector m_vBarOffset = FVector(0.f, 0.f, 120.f);

	UPROPERTY(EditDefaultsOnly, Category = "EnemyUI")
	float m_fUIVisibleDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyUI")
	bool m_bIsEnemyUIVisible = true;

	UPROPERTY(EditDefaultsOnly, Category = "EnemyUI")
	bool m_bStartHidden = true;

private:
	UPROPERTY()
	TObjectPtr<class UWidgetComponent> m_pBarWidgetComponent;

	TWeakObjectPtr<class UEnemyBarWidget> m_pBarWidget;

	bool m_bIsUIVisible = false;
	bool m_bIsHardLock = false;
	bool m_bIsDead = false;

	FTimerHandle m_UIHideTimerHandle;

	void InitializeBarWidget();
	void SetUIVisibility(bool bVisible);
	void StartHideTimer();
	void ClearHideTimer();

	UFUNCTION()
	void HandleHideTimerExpired();

};
