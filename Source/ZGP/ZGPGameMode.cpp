// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZGPGameMode.h"
#include "PlayerCharacter.h"
#include "ZGPPlayerController.h"
#include "ZGPPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AZGPGameMode::AZGPGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();

	PlayerControllerClass = AZGPPlayerController::StaticClass();

	PlayerStateClass = AZGPPlayerState::StaticClass();
}
