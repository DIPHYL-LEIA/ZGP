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

#include "ComboComponent.h"
#include "SkillComponent.h"

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

	// Combo Component
	m_pComboComp = CreateDefaultSubobject<UComboComponent>(TEXT("ComboComponent"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 두 컴포넌트의 델리게이트 바인딩
	if (m_pComboComp && m_pSkillComponent)
	{
		m_pComboComp->OnPerformComboAttack.AddDynamic(m_pSkillComponent, &USkillComponent::ExecuteComboAttack);
	}
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

void APlayerCharacter::RequestAttack()
{
	if (m_pComboComp)
	{
		m_pComboComp->RequestComboAttack();
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool APlayerCharacter::CanTagOut_Implementation() const
{
	// 젠존제에서 태그 가능/불가능한 시점 다시 알아보고 수정하기
	if (IsActionState(EActionState::ATTACKING) || IsActionState(EActionState::HIT) ||
		IsActionState(EActionState::DODGING) || IsActionState(EActionState::DEAD))
	{
		return false;
	}
	return true;
}

void APlayerCharacter::ExecuteTagIn_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	UE_LOG(LogTemp, Log, TEXT("%s : Tag In"), *GetName());

	SetActorLocation(TargetLocation);
	SetActorRotation(TargetRotation);

	// 컨트롤러 빙의 , 충돌 켜기
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	// Tag In Montage
	//PlayAnimMontage(TagInMontage);

	// Tag In 하고 Idle 
	SetActionState(EActionState::IDLE);
}

void APlayerCharacter::ExecuteTagOut_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s : Execute Tag Out "), *GetName());

	// Tag Out Montage
	//PlayAnimMontage(TagOutMontage);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}


