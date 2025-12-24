// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParryDetectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParrySuccess, AActor*, ParriedEnemy);

USTRUCT()
struct FParryableAttackInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AActor> attacker;

	UPROPERTY()
	float registerTime = 0.f;

	FParryableAttackInfo() {}
	FParryableAttackInfo(AActor* InAttacker, float InTime) 
		: attacker(InAttacker), registerTime(InTime)
	{}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UParryDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UParryDetectorComponent();

	UFUNCTION(BlueprintCallable, Category = "Parry")
	void RegisterParryableAttack(AActor* Attacker);

	UFUNCTION(BlueprintCallable, Category = "Parry")
	void UnregisterParryableAttack(AActor* Attacker);

	UFUNCTION(BlueprintPure, Category = "Parry")
	bool CanParry() const;

	UFUNCTION(BlueprintPure, Category = "Parry")
	AActor* GetParryTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Parry")
	bool ExecuteParry();

	UPROPERTY()
	FOnParrySuccess OnParrySuccess;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float m_fParryWindowDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float m_fParryMaxDistane = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float m_fParryAngle = 0.0f;

private:
	UPROPERTY()
	TArray<FParryableAttackInfo> m_arParryableAttacks;

	void CleanExpiredAttack();
	bool IsValidParryTarget(const FParryableAttackInfo& Info, const FVector& CharacterLocation, const FVector& CharacterForward, float CurrentTime) const;
	AActor* GetActiveCharacter() const;


};
