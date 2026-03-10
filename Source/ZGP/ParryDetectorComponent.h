// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParryDetectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParrySuccess, AActor*, ParriedEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UParryDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UParryDetectorComponent();

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
	float m_fParryMaxDistance = 500.f;

	UPROPERTY(EditDefaultsOnly, Category = "Parry")
	float m_fParryAngle = 0.0f;

private:
	bool IsValidParryTarget(AActor* Target, const FVector& CharacterLocation, const FVector& CharacterForward) const;
	AActor* GetActiveCharacter() const;
	void FindParryableActors(TArray<AActor*>& OutActors) const;


};
