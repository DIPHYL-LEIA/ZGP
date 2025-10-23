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

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Move 바인딩
		if (IA_Move)
		{
			EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AZGPPlayerController::HandleMove);
		}

		// Look 바인딩
		if (IA_Look)
		{
			EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AZGPPlayerController::HandleLook);
		}

		// Jump 바인딩
		if (IA_Jump)
		{
			EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Started, this, &AZGPPlayerController::HandleJump);
			EnhancedInputComponent->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AZGPPlayerController::HandleStopJumping);
		}
	}
}

void AZGPPlayerController::HandleMove(const FInputActionValue& Value)
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Move(Value);
	}
}

void AZGPPlayerController::HandleLook(const FInputActionValue& Value)
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Look(Value);
	}
}

void AZGPPlayerController::HandleJump()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->Jump();
	}
}

void AZGPPlayerController::HandleStopJumping()
{
	if (APlayerCharacter* ControlledCharacter = GetPawn<APlayerCharacter>())
	{
		ControlledCharacter->StopJumping();
	}
}



