// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TokenManagerComponent.generated.h"

UENUM(BlueprintType)
enum class ETokenPriority : uint8
{
	NORMAL		UMETA(DisplayName = "Normal"),
	ELITE		UMETA(DisplayName = "Elite Boss"),
	BOSS		UMETA(DisplayName = "Boss")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UTokenManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTokenManagerComponent();

	UFUNCTION(BlueprintCallable, Category = "Token")
	bool RequestToken(AActor* Attacker, ETokenPriority Priority = ETokenPriority::NORMAL);

	UFUNCTION(BlueprintCallable, Category = "Token")
	void ReleaseToken(AActor* Attacker);

	UFUNCTION(BlueprintPure, Category = "Token")
	bool HasToken(AActor* Attacker) const;

	UFUNCTION(BlueprintPure, Category = "Token")
	int32 GetCurrentAttackerCount() const;
		
protected:
	UPROPERTY(EditDefaultsOnly)
	int32 m_nMaxTargetAttacker = 2;

private:
	TArray<TWeakObjectPtr<AActor>> m_arTokenHolders;

	void CleanToken();

};
