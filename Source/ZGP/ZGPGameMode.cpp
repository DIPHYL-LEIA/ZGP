// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZGPGameMode.h"
#include "PlayerCharacter.h"
#include "ZGPPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AZGPGameMode::AZGPGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerCharacterClass(TEXT("/Game/ZGProject/bp-zgpPlayer.bp-zgpPlayer_c"));
	if (PlayerCharacterClass.Succeeded())
	{
		DefaultPawnClass = PlayerCharacterClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/ZGProject/bp-zgpPlayerController.bp-zgpPlayerController_c"));
	if (PlayerControllerClassFinder.Succeeded())
	{
		PlayerControllerClass = PlayerControllerClassFinder.Class;
	}
}
