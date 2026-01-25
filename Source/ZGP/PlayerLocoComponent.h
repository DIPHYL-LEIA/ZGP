// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLocoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UPlayerLocoComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerLocoComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Input
	void SetRawInput(const FVector2D& Input);
	void ClearInput();

	// Hard Lock
	void SetHardLockTarget(AActor* Target);
	void ClearHardLockTarget();

	UFUNCTION(BlueprintPure)
	bool IsHardLock() const { return m_pHardLockTarget.IsValid(); }

	// Attack Homing
	void StartAttackHoming(AActor* Target);
	void StopAttackHoming();

	UFUNCTION(BlueprintPure)
	bool IsAttackHoming() const { return m_bIsAttackHoming; }

protected:
	virtual void BeginPlay() override;
		
private:
	// 소유자 캐시
	TWeakObjectPtr<ACharacter> m_pOwnerCharacter;

	// Input
	FVector2D m_vRawInput = FVector2D::ZeroVector;
	FVector2D m_vSmoothInput = FVector2D::ZeroVector;

	// 방향 전환
	FVector m_vLastDirection = FVector::ZeroVector;
	float m_fCurrentSpeedMultiply = 1.0f;

	// Hard Lock
	TWeakObjectPtr<AActor> m_pHardLockTarget;

	// Attack Homing
	bool m_bIsAttackHoming = false;
	TWeakObjectPtr<AActor> m_pHomingTarget;
	FVector m_vHomingTargetLocation = FVector::ZeroVector;

	// 이동 보간 파라미터
	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fInterpInputSpeed = 8.0;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fInterpRotationSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fMinSpeedDirectionChange = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fBackSpeed = 5.0f;

	// Attack Homing 파라미터
	UPROPERTY(EditDefaultsOnly)
	float m_fHomingDistance = 300.f;

	UPROPERTY(EditDefaultsOnly)
	float m_fHomingSpeed = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float m_fHomingStopDistance = 100.f;

	// 함수
	void UpdateNormalMovement(float DeltaTime);
	void UpdateHardLockMovement(float DeltaTime);
	void UpdateAttackHoming(float DeltaTime);

	void UpdateInputSmoothing(float DeltaTime);
	void UpdateDirectionChange(float DeltaTime, const FVector& CurrentDirection);
	void ApplyMovement(const FVector& MoveDirection, float SpeedMultiplier);

	FVector CalculateMoveDirection() const;
	FVector GetTargetLocation(AActor* Target) const;
	void RotateCharacterToTarget(AActor* Target, float DeltaTime, bool bInstant);


};
