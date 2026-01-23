// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackData.h"
#include "EnemyFlashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlashStart, EAttackFlashType, FlashType, FName, AttackID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlashEnd);

/*
* 패리 윈도우 관리는 AnimNotifyState_ParryWindow가 담당
* 시각 담당
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyFlashComponent();

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void StartFlash(EAttackFlashType FlashType, FName AttackID = NAME_None, float Duration = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Flash")
	void StopFlash();

	// 상태
	UFUNCTION(BlueprintPure, Category = "Flash")
	bool IsFlashing() const { return m_bIsFlashing; }

	UFUNCTION(BlueprintPure, Category = "Flash")
	EAttackFlashType GetCurrentFlashType() const { return m_eCurrentFlashType; }

	UFUNCTION(BlueprintPure, Category = "Flash")
	bool IsParryableFlash() const { return m_bIsFlashing && m_eCurrentFlashType == EAttackFlashType::GOLD; }

	UFUNCTION(BlueprintPure, Category = "Flash")
	bool IsUnblockableFlash() const { return m_bIsFlashing && m_eCurrentFlashType == EAttackFlashType::RED; }

	UPROPERTY()
	FOnFlashStart OnFlashStart;

	UPROPERTY()
	FOnFlashEnd	OnFlashEnd;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 나이아가라 이펙트
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraSystem> m_pGoldFlashSystem;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> m_pRedFlashSystem;

	UPROPERTY(EditDefaultsOnly)
	FName m_EffectAttachSocket = NAME_None;		// 부착 소켓 위치

	UPROPERTY(EditDefaultsOnly)
	FVector m_EffectOffset = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditDefaultsOnly)
	FVector m_EffectScale = FVector(1.f);

	// 사운드
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> m_pGoldFlashSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> m_pRedFlashSound;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float m_fSoundVolumeMultiplier = 1.0f;

	// 타이밍
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1"))
	float m_fDefaultFlashDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	bool m_bUseAutoStop = true;					// 자동 종료 사용 여부

	// 자동 구독(EnemySkillComponent)
	UPROPERTY(EditDefaultsOnly)
	bool m_bAutoBindSkillComponent = true;

private:
	bool m_bIsFlashing = false;;
	EAttackFlashType m_eCurrentFlashType = EAttackFlashType::NONE;
	FName m_CurrentAttackID = NAME_None;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> m_pNiagaraComponent;

	FTimerHandle m_FlashTimerHandle;

	void SpawnNiagaraEffect(EAttackFlashType FlashType);
	void CleanNiagaraEffect();
	void PlayFlashSound(EAttackFlashType FlashType);
	void OnFlashTimeExpired();
	void BindSkillComponent();

	UFUNCTION()
	void HandleAttackStarted(FName AttackID);

	EAttackFlashType GetFlashTypeAttack(FName AttackID) const;
};