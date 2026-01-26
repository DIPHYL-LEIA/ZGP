// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCameraComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Hard Lock
	void SetHardLockTarget(AActor* Target);
	void ClearHardLockTarget();

	UFUNCTION(BlueprintPure, Category = "Camera")
	bool IsHardLock() const { return m_pHardLockTarget.IsValid(); }

	// Camera Lag
	void StopCameraLagTemporary();
	void RestoreCameraLag();

	// Spring Arm
	void SetSpringArm(class USpringArmComponent* SpringArm);

protected:
	virtual void BeginPlay() override;
		
private:
	// Target
	TWeakObjectPtr<AActor> m_pHardLockTarget;

	// Spring Arm
	TWeakObjectPtr<USpringArmComponent> m_pSpringArmComponent;

	// 원본 세팅
	bool m_bOriginalCameraLag = true;
	bool m_bOriginalCollision = true;
	bool m_bCameraLagDisabled = false;

	// 파라미터
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float m_fHardLockRotationSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float m_fPitchClampMin = -30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float m_fPitchClampMax = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float m_fMinDistanceForPitch = 600.f;  // 이 거리 이하에서 Pitch 변화 억제

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float m_fDefaultPitch = -30.f;  // 기본 Pitch (약간 아래를 봄)

	// 함수
	void UpdateHardLockCamera(float DeltaTime);
	FVector GetTargetLocation(AActor* Target) const;
};
