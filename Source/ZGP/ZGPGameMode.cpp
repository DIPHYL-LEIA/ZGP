// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZGPGameMode.h"
#include "ZGPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AZGPGameMode::AZGPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
