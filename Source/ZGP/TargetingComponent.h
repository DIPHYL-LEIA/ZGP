// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

UENUM(BlueprintType)
enum class ETargetingMode : uint8
{
	NONE		UMETA(DisplayName = "None"),
	SOFT_LOCK	UMETA(DisplayName = "Soft Lock"),
	HARD_LOCK	UMETA(DisplayName = "Hard Lock")
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void UpdateTargeting(const FVector& Location, const FVector& Forward, AActor* IgnoreActor);

	UFUNCTION(BlueprintPure, Category = "Targeting")
	AActor* GetCurrentTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void SetCurrentTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Targeting")
	ETargetingMode GetTargetingMode() const;

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ToggleLockMode();

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ToggleLockOn();

protected:
	float CalculateScore(AActor* TargetActor, const FVector& Location, const FVector& Forward);
	void ChangeTarget(AActor* NewTarget);

	void UpdateSoftLock(const FVector& Location, const FVector& Forward, AActor* IgnoreActor);
	void UpdateHardLock(const FVector& Location);

	TArray<AActor*> FindTargetInRange(const FVector& Location, AActor* IgnoreActor);

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	ETargetingMode m_eTargetingMode = ETargetingMode::SOFT_LOCK;

	TWeakObjectPtr<AActor> m_pCurrentTarget;
	float m_fLastUpdateTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSoftLockInterval = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHardLockValidInterval = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSearchRadius = 1000.f;

	// 1(0도) ~ -1(180도)
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSearchAngle = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fDistanceWeight = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fAngleWeight = 0.6f;

	// 현재 타겟 유지 가산점(타겟 계속 바뀌는 거 방지 - 히스테리시스)
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHysteresisValue = 0.15f;

	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	bool m_bIsLockOn;


	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool m_bShowDebugShape;

};
