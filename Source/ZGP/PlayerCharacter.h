// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Taggable.h"
#include "Targetable.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class ZGP_API APlayerCharacter : public ABaseCharacter, public ITaggable, public ITargetable
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	void Move(const struct FInputActionValue& Value);
	void StopMove();
	void Look(const FInputActionValue& Value);

	void RequestAttack();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Tag
	virtual bool CanTag_Implementation() const override;
	virtual void OnTagIn_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;
	virtual void OnTagOut_Implementation() override;
	virtual void OnTagOutAction_Implementation() override;

	// Target
	virtual bool IsTargetable_Implementation() const override;
	virtual FVector GetTargetLocation_Implementation() const override;
	virtual void OnTargeted_Implementation(bool IsTargeted) override;
	virtual void OnUnTargeted_Implementation() override;

	// Dodge
	virtual void ApplyCombatEffect_Implementation(const FDamageData& DamageData) override;
	void RequestDodge();

	UFUNCTION(BlueprintCallable, Category = "Parry")
	void RequestParryAttack(AActor* ParriedEnemy);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> m_SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> m_Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UComboComponent> m_pComboComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDodgeComponent> m_pDodgeComp;

	UPROPERTY(EditDefaultsOnly)
	FName m_ParryAttackSkillID = FName("ParryAttack");

	UPROPERTY(EditDefaultsOnly)
	float m_fForceTagOutDelay = 3.0f;

	// 이동 보간
	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fInterpInputSpeed = 8.0;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fInterpRotationSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fMinSpeedDirectionChange = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "MovementSmoothing")
	float m_fBackSpeed = 5.0f;

private:
	// 태그 시 남은 행동 처리
	bool m_bPendingTagOut = false;
	FTimerHandle ForceTagOutTimerHandle;

	// 이동 보간 시스템
	// 입력
	FVector2D m_vRawInput = FVector2D::ZeroVector;
	FVector2D m_vSmoothedInput = FVector2D::ZeroVector;

	// 회전
	FVector m_vLastDirection = FVector::ZeroVector;

	float m_fCurrentSpeedMultiply = 1.0f;

	UFUNCTION()
	void HandleActionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ExecuteActionTagOut();
	void SetActionTagOutState(bool bActive);

	// Camera Lag
	bool m_bCameraLag = false;
	bool m_bCameraCollision = true;
	bool m_bCameraResetPending = false;
	FTimerHandle m_CameraLagTimerHandle;

	UFUNCTION()
	void ResetCameraSetting();

	// 이동 보간 함수
	void UpdateMovementSmoothing(float DeltaTime);
	void UpdateInputSmoothing(float DeltaTime);
	void UpdateDirectionChange(float DeltaTime, const FVector& CurrentDirection);
	void UpdateRotationSmoothing(float DeltaTime, const FVector& MoveDirection);
	void ApplyMovementSmoothing(const FVector& MoveDirection);

};
