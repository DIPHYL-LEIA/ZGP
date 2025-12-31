// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageData.h"
#include "HitReactionComponent.generated.h"

UENUM(BlueprintType)
enum class EHitDirection : uint8
{
	FRONT	UMETA(DisplayName = "Front"),
	BACK	UMETA(DisplayName = "Back"),
	LEFT	UMETA(DisplayName = "Left"),
	RIGHT	UMETA(DisplayName = "Right")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHitReactionStart, EHitReactionType, ReactionType, bool, bCancel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHitReactionEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHitReactionComponent();

	UFUNCTION(BlueprintCallable, Category = "HitReaction")
	void RunHitReaction(const FDamageData& DamageData);

	UPROPERTY(BlueprintAssignable, Category = "HitReaction")
	FOnHitReactionStart OnHitReactionStart;

	UPROPERTY(BlueprintAssignable, Category = "HitReaction")
	FOnHitReactionEnd OnHitReactionEnd;

protected:
	// 府咀记 根鸥林
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> m_pHitLightMontage;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> m_pHitHeavyMontage;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<UAnimMontage> m_pKnockdownMontage;

	// 乘归
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	float m_fLightKnockback = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	float m_fHeavyKnockback = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	float m_fLaunchVerticalRatio = 0.5f;
		
private:
	UPROPERTY()
	TObjectPtr<UAnimMontage> m_pCurrentHitMontage;

	EHitDirection CalculateHitDirection(const FVector& HitDirection) const;
	void PlayHitMontage(EHitReactionType ReactionType, EHitDirection Direction);
	void ApplyKnockback(const FVector& HitDirection, EHitReactionType ReactionType);

	static FName GetSectionName(EHitDirection Direction);

	UFUNCTION()
	void HandleMontageEnd(UAnimMontage* Montage, bool bInterrupted);

};
