// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UEnemyBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EnemyBar")
	void UpdateHealth(float Percent);

	UFUNCTION(BlueprintCallable, Category = "EnemyBar")
	void UpdateDaze(float Percent);

	UFUNCTION(BlueprintCallable, Category = "EnemyBar")
	void SetHealthNow(float Percent);

	UFUNCTION(BlueprintCallable, Category = "EnemyBar")
	void SetDazeNow(float Percent);

	UFUNCTION(BlueprintCallable, Category = "EnemyBar")
	void UpdateDazeValue(float CurrentDaze, float MaxDaze);

protected:
	virtual void NativeConstruct() override;

	// 블루프린트에서 할당
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UBarWidget> HPBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UBarWidget> DazeBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> DazeValue;
};
