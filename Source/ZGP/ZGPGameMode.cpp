// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZGPGameMode.h"
#include "PlayerCharacter.h"
#include "ZGPPlayerController.h"
#include "ZGPPlayerState.h"
#include "UObject/ConstructorHelpers.h"

AZGPGameMode::AZGPGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerCharacterClass(TEXT("/Game/bp-zgpPlayer.bp-zgpPlayer_c"));
	if (PlayerCharacterClass.Succeeded())
	{
		DefaultPawnClass = PlayerCharacterClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Player"));
		DefaultPawnClass = APlayerCharacter::StaticClass();
	}


	PlayerControllerClass = AZGPPlayerController::StaticClass();

	PlayerStateClass = AZGPPlayerState::StaticClass();
}
