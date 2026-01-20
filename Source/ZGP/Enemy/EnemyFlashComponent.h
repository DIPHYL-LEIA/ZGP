// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackData.h"
#include "EnemyFlashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttackFlashTrigger, AActor*, Attacker, EAttackFlashType, FlashType, bool, bIsParryable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackFlashEnd, AActor*, Attacker);

/*
* 패리 윈도우 관리는 AnimNotifyState_ParryWindow가 담당
* 시각적 피드백만 담당
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFlashComponent();

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void TriggerFlash(EAttackFlashType FlashType, float Duration = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void EndFlash();

	UFUNCTION(BlueprintPure, Category = "Flash")
	EAttackFlashType GetCurrentFlash() const { return m_eCurrentFlashType; }

	UFUNCTION(BlueprintPure, Category = "Flash")
	bool IsFlashActive() const { return m_bFlashActive; }

	UFUNCTION(BlueprintPure, Category = "Flash")
	bool IsParryable() const { return m_bFlashActive && m_eCurrentFlashType == EAttackFlashType::GOLD; }

	UPROPERTY(BlueprintAssignable)
	FOnAttackFlashTrigger OnAttackFlashTrigger;

	UPROPERTY(BlueprintAssignable)
	FOnAttackFlashEnd OnAttackFlashEnd;


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* 나이아가라 이펙트 */
	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> m_pGoldFlashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> m_pRedFlashEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	FName m_EffectSocketName = TEXT("WeaponSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Niagara")
	FVector m_EffectOffset = FVector(0.f, 0.f, 100.f);

	/* 머티리얼(선택) */

	/* 사운드 */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> m_pGoldFlashSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> m_pRedFlashSound;

	/* 지속 시간 */
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1"))
	float m_fDefaultFlashDuration = 0.5f;

private:
	void StartNiagaraEffect(EAttackFlashType FlashType);
	void StopNiagaraEffect();
	void PlayFlashSound(EAttackFlashType FlashType);

	bool m_bFlashActive = false;
	EAttackFlashType m_eCurrentFlashType = EAttackFlashType::NONE;
	float m_fCurrentDuration = 0.f;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> m_pActiveNiagaraComponent;

};