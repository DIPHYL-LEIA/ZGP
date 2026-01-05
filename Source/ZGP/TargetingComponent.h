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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*, NewTarget);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetingComponent();

	void UpdateTargeting(float DeltaTime, const FVector& Location, const FVector& Forward, AActor* IgnoreActor);

	UFUNCTION(BlueprintCallable, Category = "Targeting")
	void ToggleLockOn();

	UFUNCTION(BlueprintPure, Category = "Targeting")
	AActor* GetCurrentTarget() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	ETargetingMode GetTargetingMode() const;

	UFUNCTION(BlueprintPure, Category = "Targeting")
	bool IsHardLock() const;

	UPROPERTY(BlueprintAssignable, Category = "Targeting")
	FOnTargetChanged OnTargetChanged;


protected:
	void SetSoftLock(const FVector& Location, const FVector& Forward, AActor* IgnoreActor);
	void SetHardLock(float DeltaTime, const FVector& Location);

	float CalculateScore(AActor* TargetActor, const FVector& Location, const FVector& Forward);
	bool IsTargetVisible(AActor* TargetActor, const FVector& StartLocation) const;
	bool IsInViewAngle(AActor* TargetActor, const FVector& Location, const FVector& Forward) const;
	void ChangeTarget(AActor* NewTarget);
	void ReleaseLock();

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Targeting")
	ETargetingMode m_eTargetingMode = ETargetingMode::SOFT_LOCK;

	TWeakObjectPtr<AActor> m_pCurrentTarget;

	// Throttling
	float m_fSoftLockTimer = 0.f;
	float m_fHardLockTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSoftLockInterval = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHardLockInterval = 0.3f;

	// 탐색 세팅
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSearchRadius = 1000.f;

		// 1(0도) ~ -1(180도)
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fSearchAngle = 0.5f;

	// 가중치
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fDistanceWeight = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fAngleWeight = 0.6f;

		// 현재 타겟 유지 가산점(타겟 계속 바뀌는 거 방지 - 히스테리시스)
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHysteresisValue = 0.15f;

	// Hard Lock
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHardLockRelease = 1.5f;

	float m_fLostTargetTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float m_fHardLockLostTarget = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool m_bShowDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TEnumAsByte<ECollisionChannel> m_eTargetChannel = ECC_GameTraceChannel2;	// Enemy

};
