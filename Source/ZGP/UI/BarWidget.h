// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarWidget.generated.h"

/**
 * 체력/Daze 등의 바 형태 UI
 */
UCLASS(Abstract)
class ZGP_API UBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UBarWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetPercent(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetPercentNow(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetUseRealTime(bool bUseRealTime);

	// Bar 색상 동적 변경
	UFUNCTION(BlueprintCallable, Category = "Bar")
	void SetBarColor(FLinearColor NewColor);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> CurrentBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GhostBar;

	//
	UPROPERTY(EditDefaultsOnly, Category = "Bar")
	TObjectPtr<class UMaterialInterface> m_pBarMaterial;

	// 보간 속도
	UPROPERTY(EditDefaultsOnly, Category = "Bar")
	float m_fGhostInterpSpeed = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Bar")
	float m_fGhostTolerance = 0.001f;

	UPROPERTY(EditDefaultsOnly, Category = "Bar")
	float m_fGhostDelay = 0.3f;

	// Daze용 Ghost 반전
	UPROPERTY(EditDefaultsOnly, Category = "Bar")
	bool m_bInvertGhost = false;

	// Bar 색상 
	UPROPERTY(EditDefaultsOnly, Category = "Bar|Style")
	FLinearColor m_CurrentBarColor = FLinearColor(0.0f, 0.85f, 0.65f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Bar|Style")
	FLinearColor m_GhostBarColor = FLinearColor(0.8f, 0.1f, 0.1f, 1.0f);

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<class UMaterialInstanceDynamic> m_pCurrentBarID;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UMaterialInstanceDynamic> m_pGhostBarID;

private:
	float m_fCurrentPercent = 1.0f;
	float m_fTargetPercent = 1.0f;
	float m_fGhostPercent = 1.0f;
	float m_fGhostDelayTimer = 0.0f;
	bool m_bGhostDelayActive = false;
	bool m_bUseRealTime = false;
	bool m_bIsTickEnabled = false;


	void InitializeMaterials();

	void EnableTick(bool bEnable);
	void UpdateGhostBar(float DeltaTime);
	float GetDeltaTime(float InDeltaTime) const;

	static const FName PARAM_PERCENT;
	static const FName PARAM_COLOR;

};
