// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "ZGPPlayerController.h"
#include "InputAction.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, -0.f));

	m_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	m_SpringArm->SetupAttachment(RootComponent);
	m_SpringArm->TargetArmLength = 400.f;
	m_SpringArm->bUsePawnControlRotation = true;

	m_SpringArm->SocketOffset = FVector(0.f, 50.f, 70.f);
	m_SpringArm->bEnableCameraLag = true;
	m_SpringArm->CameraLagSpeed = 10.f;

	m_SpringArm->bDoCollisionTest = true;

	m_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	m_Camera->SetupAttachment(m_SpringArm);
	m_Camera->bUsePawnControlRotation = false;

	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AZGPPlayerController* PlayerController = Cast<AZGPPlayerController>(GetController()))
		{
			if (PlayerController->IA_Move)
			{
				EnhancedInputComponent->BindAction(PlayerController->IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
			}

			if (PlayerController->IA_Look)
			{
				EnhancedInputComponent->BindAction(PlayerController->IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
			}

			if (PlayerController->IA_Jump)
			{
				EnhancedInputComponent->BindAction(PlayerController->IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
				EnhancedInputComponent->BindAction(PlayerController->IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			}
		}
	}
}

void APlayerCharacter::OnTagIn_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCharacterMovement()->Activate();

	UE_LOG(LogTemp, Warning, TEXT("%s Tagged In"), *GetName());
}

void APlayerCharacter::OnTagOut_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	GetCharacterMovement()->Deactivate();

	UE_LOG(LogTemp, Warning, TEXT("%s Tagged Out"), *GetName());
}
