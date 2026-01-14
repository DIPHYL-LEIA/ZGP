// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionState.h"
#include "ComboData.h"
#include "ComboComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPerformComboAttack, int32, ComboIndex, bool, bIsLastCombo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResetCombo);

/// <summary>
/// 
/// Combo Component
/// Basic Attack의 콤보 체인 및 Tap/Hold 분기 관리
/// 
/// 입력, Tap, Hold  관리
/// 콤보 윈도우 관리
/// ComboData 기반 다음 콤보 결정
/// SkillComponent에 실행 요청
/// 
/// </summary>
/// <param name=""></param>
/// <param name=""></param>
/// <param name=""></param>
/// 
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TObjectPtr<UDataTable> m_pComboDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FName m_ComboDataRowName;

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	float m_fDefaultHoldTime = 0.5f;

	// Runtime
	UPROPERTY(VisibleInstanceOnly, Category = "Combo")
	int32 m_nCurrentComboIndex = 0;

	UPROPERTY(VisibleInstanceOnly, Category = "Combo")
	bool m_bIsInComboWindow = false;

	UPROPERTY(VisibleInstanceOnly, Category = "Combo")
	bool m_bIsComboActive = false;

	// 다음 입력 대기인지
	bool m_bPendingInput = false;
	bool m_bIsAttackPressed = false;
	float m_fAttackPressTime = 0.f;
	float m_fComboResetTimer = 0.f;
	EInputAttackType m_ePendingInputType = EInputAttackType::TAP;
	const FComboData* m_pCachedComboData = nullptr;	


	FTimerHandle ComboResetTimerHandle;

public:
	UPROPERTY(BlueprintAssignable, Category = "Combo")
	FOnPerformComboAttack	OnPerformComboAttack;
	FOnResetCombo			OnResetCombo;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UComboComponent();
	
	// Combo Input
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void OnAttackPressed();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void OnAttackReleased();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void RequestComboAttack();
	
	// Combo Window
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void StartComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void EndComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();

	// Combo State
	UFUNCTION(BlueprintPure, Category = "Combo")
	int32 GetCurrentComboIndex() const { return m_nCurrentComboIndex; }

	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsInComboWindow() const { return m_bIsInComboWindow; }

	UFUNCTION(BlueprintPure, Category = "Combo")
	bool IsComboActive() const { return m_bIsComboActive; }


private:
	void LoadComboData();
	void ExecuteCombo(EInputAttackType InputType);
	float GetCurrentHoldTime() const;

	void StartResetTimer();
	void ClearResetTimer();
	void OnResetTimerExpired();

};
