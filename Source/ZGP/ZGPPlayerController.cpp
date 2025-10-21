// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AZGPPlayerController::AZGPPlayerController()
{
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-move.ia-move"));
	if (MoveActionAsset.Succeeded())
	{
		IA_Move = MoveActionAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-look.ia-look"));
	if (LookActionAsset.Succeeded())
	{
		IA_Look = LookActionAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionAsset(TEXT("/Game/ZGProject/ZGPInput/ia-jump.ia-jump"));
	if (JumpActionAsset.Succeeded())
	{
		IA_Jump = JumpActionAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultMappingContextAsset(TEXT("/Game/ZGProject/ZGPInput/imc-default.imc-default"));
	if (DefaultMappingContextAsset.Succeeded())
	{
		IMC_Default = DefaultMappingContextAsset.Object;
	}
}

void AZGPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (IMC_Default)
		{
			Subsystem->AddMappingContext(IMC_Default, DefaultInputMappingPriority);
		}
	}
}

void AZGPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


}


