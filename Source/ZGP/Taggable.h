// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Taggable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTaggable : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ITaggable
{
	GENERATED_BODY()

public:
	// ex) 피격, 스킬 시전 중이 아닐 때 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tag")
	bool CanTag() const;
	// ex) 이펙트 재생, 액터 숨길 때 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tag")
	void OnTagIn(const FVector& TargetLocation, const FRotator& TargetRotation);
	// ex) 지정된 위치에 태그, 이펙트 재생, 액터 나타날 때 
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tag")
	void OnTagOut();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tag")
	void OnTagOutAction();
};
