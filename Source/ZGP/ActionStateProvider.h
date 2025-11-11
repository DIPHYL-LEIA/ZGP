// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActionState.h"
#include "ActionStateProvider.generated.h"

UINTERFACE(MinimalAPI)
class UActionStateProvider : public UInterface
{
	GENERATED_BODY()
};

class ZGP_API IActionStateProvider
{
	GENERATED_BODY()

public:
	virtual bool CanChangeActionState(EActionState NewState) const = 0;
	virtual void SetActionState(EActionState NewState) const = 0;
	virtual bool IsActionState(EActionState State) const = 0;
};
