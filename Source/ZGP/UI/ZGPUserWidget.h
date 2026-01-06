// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIType.h"
#include "ZGPUserWidget.generated.h"

UCLASS()
class ZGP_API UZGPUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UZGPUserWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	virtual void ShowWidget();

	UFUNCTION(BlueprintCallable)
	virtual void HideWidget();

	UFUNCTION()
	bool IsWidgetVisible() const { return m_bIsVisible; }


protected:
	UPROPERTY(EditAnywhere)
	bool m_bStartHidden = true;

	UPROPERTY(EditAnywhere)
	ESlateVisibility m_eShowVisibility = ESlateVisibility::SelfHitTestInvisible;

	// UMG (In BP)
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnimation;

	UPROPERTY(EditDefaultsOnly)
	float m_fAnimationSpeed = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float m_fGhostBarDecreaseSpeed = 3.0f;

	UPROPERTY(EditDefaultsOnly)
	float m_fGhostBarIncreaseSpeed = 8.0f;

	UPROPERTY(EditDefaultsOnly)
	float m_fGaugeTolerance = 0.001f;

	// UserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Ghost Bar
	//// 보간
	UFUNCTION()
	float InterpGaugeValue(float Current, float Target, float DeltaTime, float Speed = 5.0f);

	//// 보간 (방향별 속도) 
	UFUNCTION()
	float InterpGaugeDirection(float Current, float Target, float DeltaTime, float DecreaseSpeed = 3.0f, float IncreaseSpeed = 8.0f);

	UFUNCTION()
	E_UI_GAUGEBAR GetGaugeBar(float Current, float Target, float Tolerance = 0.001f) const;

	// ???
	UFUNCTION()
	bool UpdateGhostBar(UPARAM(ref) float& OutCurrent, UPARAM(ref) float& OutGhost, float Target, float DeltaTime, E_UI_GAUGEBAR& OutBar);

	UFUNCTION()
	void SetWidgetTickEnabled(bool bEnable);

	UFUNCTION()
	bool IsWidgetTickEnabled() const { return m_bTickEnabled; }

	// 
	virtual void OnShowWidget() {}
	virtual void OnHideWidget() {}
	virtual void UpdateWidget(float DeltaTime) {}

private:
	bool m_bIsVisible = false;
	bool m_bTickEnabled = false;

	// 애니메이션 완료 핸들
	void HandleShowAnimationFinished();
	void HandleHideAnimationFinished();

	FWidgetAnimationDynamicEvent m_ShowAnimFinishedDelegate;
	FWidgetAnimationDynamicEvent m_HideAnimFinishedDelegate;


};
