// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "../Enemy/EnemyAttackData.h"
#include "AnimNotify_EnemyFlash.generated.h"

UCLASS()
class ZGP_API UAnimNotify_EnemyFlash : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flash")
	EAttackFlashType FlashType = EAttackFlashType::GOLD;
};
