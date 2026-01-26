// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraEffectType.h"
#include "CameraEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraEffectStart, ECameraEffectType, EffectType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraEffectEnd, ECameraEffectType, EffectType);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UCameraEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraEffectComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 실행
	UFUNCTION(BlueprintCallable, Category = "CameraEffect")
	void PlayEffect(ECameraEffectType EffectType, AActor* FocusTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CameraEffect")
	void PlayCustomEffect(const FCameraEffectParams& Params, AActor* FocusTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category = "CameraEffect")
	void StopEffect();

	//
	UFUNCTION(BlueprintPure, Category = "CameraEffect")
	bool IsPlayEffect() const { return m_bIsPlay; }

	UFUNCTION(BlueprintPure, Category = "CameraEffect")
	ECameraEffectType GetCurrentEffectType() const { return m_eCurrentEffectType; }

	UFUNCTION(BlueprintPure, Category = "CameraEffect")
	bool IsSlowMotionActive() const { return m_bSlowMotionActive; }

	void ExcludeCurrentPawn();

	// Parry
	UFUNCTION(BlueprintCallable)
	void StartParrySequence(AActor* ParriedEnemy);

	UFUNCTION(BlueprintCallable)
	void StartParryImpact();

	UFUNCTION(BlueprintCallable)
	void EndParrySequence();

	UFUNCTION(BlueprintCallable)
	bool IsParrySequenceActive() const { return m_bParrySequenceActive; }

	UPROPERTY(BlueprintAssignable, Category = "CameraEffect")
	FOnCameraEffectStart OnCameraEffectStart;

	UPROPERTY(BlueprintAssignable, Category = "CameraEffect")
	FOnCameraEffectEnd OnCameraEffectEnd;


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	FCameraEffectParams m_PerfectDodgeParams;

	UPROPERTY(EditDefaultsOnly)
	FCameraEffectParams m_ParryAssistParams;

	UPROPERTY(EditDefaultsOnly)
	FCameraEffectParams m_ChainAttackParams;

private:
	bool m_bIsPlay = false;
	ECameraEffectType m_eCurrentEffectType = ECameraEffectType::NONE;
	FCameraEffectParams m_CurrentParams;
	TWeakObjectPtr<AActor> m_pFocusTarget;

	float m_fEffectTimer = 0.f;
	float m_fEffectDuration = 0.f;

	float m_fOriginalTimeDelay = 1.0f;
	float m_fOriginalFOV = 90.f;

	bool m_bSlowMotionActive = false;
	bool m_bExcludePlayerFromSlow = false;
	TWeakObjectPtr<APawn> m_pExcludedPawn;

	// FOV 보간
	bool m_bIsZoom = false;
	float m_fCurrentFOV = 90.f;
	float m_fTargetFOV = 90.f;

	UPROPERTY(EditDefaultsOnly)
	float m_fZoomInterpSpeed = 8.0f;

	// Parry
	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	FParrySequenceParams m_ParrySequenceParams;

	bool m_bParrySequenceActive = false;
	TWeakObjectPtr<AActor> m_pParriedEnemy;

	FTimerHandle m_ParryImpactTimerHandle;

	void EndParryImpact();

	void StartEffect(const FCameraEffectParams& Params, AActor* FocusTarget);
	void EndEffect();

	void ApplySlowMotion(float Scale, bool bExcludePlayer);
	void RestoreSlowMotion();

	void ApplyZoom(float FOVOffset);
	void RestoreZoom();

	void ApplyShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale);

	void ApplyFocus(AActor* Target);
	void RestoreFocus();

	const FCameraEffectParams* GetParams(ECameraEffectType EffectType) const;
	APlayerController* GetPlayerController() const;
	APawn* GetActivePawn() const;

	void UpdateZoom(float DeltaTime);
};
