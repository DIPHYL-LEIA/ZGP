// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ParryImpact.generated.h"

/**
 * 
 */
UCLASS()
class ZGP_API UAnimNotify_ParryImpact : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_ParryImpact();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;
};
