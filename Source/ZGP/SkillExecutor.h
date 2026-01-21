// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SkillExecutor.generated.h"

/**
 * 
 */

UINTERFACE(MinimalAPI, BlueprintType)
class USkillExecutor : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API ISkillExecutor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	bool ExecuteSkillByID(FName SkillID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	bool IsExecuteSkill() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	bool IsCurrentSkillHeavy() const;

	// 스킬 완료 알림(몽타주 종료 시 BaseCharacter에서 호출)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	void NotifySkillCompleted();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Skill")
	FName GetCurrentSkillID() const;
};
