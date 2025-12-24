// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DodgeComponent.generated.h"

UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	FORWARD		UMETA(DisplayName = "Forward"),
	BACKWARD	UMETA(DisplayName = "Backward"),
	LEFT		UMETA(DisplayName = "Left"),
	RIGHT		UMETA(DisplayName = "Right")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodgeStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodgeEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPerfectDodge);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UDodgeComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool m_bIsInvincible = false;
	bool m_bIsDodging = false;
	bool m_bPerfectDodgeWindowActive = false;
	float m_fLastDodgeTime = -999.f;

public:	
	UDodgeComponent();

	UFUNCTION(BlueprintCallable, Category = "Dodge")
	bool RequestDodge(const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "Dodge")
	void SetInvincible(bool bInvincible);

	UFUNCTION(BlueprintPure, Category = "Dodge")
	bool IsInvincible() const { return m_bIsInvincible; }

	// Perfect Dodge
	UFUNCTION(BlueprintCallable, Category = "Dodge")
	void SetPerfectDodgeWindow(bool bActive);

	// Perfect Dodge ÆÇÁ¤
	UFUNCTION(BlueprintCallable, Category = "Dodge")
	bool TryPerfectDodgeTrigger();

	UPROPERTY(BlueprintAssignable)
	FOnDodgeStart OnDodgeStart;

	UPROPERTY(BlueprintAssignable)
	FOnDodgeEnd OnDodgeEnd;

	UPROPERTY(BlueprintAssignable)
	FOnPerfectDodge OnPerfectDodge;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	TObjectPtr<UAnimMontage> m_pDodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float m_fDodgeDistance = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float m_fDodgeCooldown = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float m_fPerfectDodgeSlowDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dodge")
	float m_fPerfectDodgeSlowRate = 0.1f;

private:
	void ExecuteDodge(const FVector& Direction);

	EDodgeDirection CalculateDodgeDirection(const FVector& InputDirection) const;
	static FName GetDodgeSectionName(EDodgeDirection Direction);

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void ActivateDodgeEffect();
	void DeActivateDodgeEffect();

	class IActionStateProvider* GetActionStateProvider() const;

	FTimerHandle PerfectDodgeTimerHandle;


};
