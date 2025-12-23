// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionStateProvider.h"
#include "ActionState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UDodgeComponent::UDodgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDodgeComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool UDodgeComponent::RequestDodge(const FVector& Direction)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - m_fLastDodgeTime < m_fDodgeCooldown) return false;

	IActionStateProvider* Provider = GetActionStateProvider();
	if (!Provider) return false;

	if (!Provider->CanChangeActionState(EActionState::DODGING)) return false;

	// 회피 실행
	m_fLastDodgeTime = CurrentTime;
	ExecuteDodge(Direction);

	return true;
}

void UDodgeComponent::SetInvincible(bool bInvincible)
{
	m_bIsInvincible = bInvincible;
}

void UDodgeComponent::SetPerfectDodgeWindow(bool bActive)
{
	m_bPerfectDodgeWindowActive = bActive;
}

bool UDodgeComponent::TryPerfectDodgeTrigger()
{
	if (!m_bPerfectDodgeWindowActive) return false;
	if (!m_bIsDodging) return false;

	ActivateDodgeEffect();
	OnPerfectDodge.Broadcast();

	return true;
}

void UDodgeComponent::ExecuteDodge(const FVector& Direction)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	IActionStateProvider* Provider = GetActionStateProvider();
	if (!Provider) return;

	m_bIsDodging = true;
	Provider->SetActionState(EActionState::DODGING);

	OnDodgeStart.Broadcast();

	FVector DodgeDirection = Direction;
	if (DodgeDirection.IsNearlyZero())
	{
		// 회피 방향으로의 입력이 없으면 후방으로 이동
		DodgeDirection = -Owner->GetActorForwardVector();
	}

	DodgeDirection.Z = 0.f;
	DodgeDirection.Normalize();

	FRotator TargetRotation = DodgeDirection.Rotation();
	Owner->SetActorRotation(TargetRotation);

	// 이동 움직임 적용
	ApplyDodge(DodgeDirection);

	if (m_pDodgeMontage)
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UDodgeComponent::HandleMontageEnded);
			AnimInstance->OnMontageEnded.AddDynamic(this, &UDodgeComponent::HandleMontageEnded);
		}
		Owner->PlayAnimMontage(m_pDodgeMontage);
	}
}

void UDodgeComponent::ApplyDodge(const FVector& Direction)
{
	// 회피 움직임
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	UCharacterMovementComponent* MovementComponent = Owner->GetCharacterMovement();	
	if (!MovementComponent) return;

	// Root Motion 사용 시 생략 가능
	FVector LaunchVelocity = Direction * m_fDodgeDistance;
	Owner->LaunchCharacter(LaunchVelocity, true, true);

}

void UDodgeComponent::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		if (UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UDodgeComponent::HandleMontageEnded);
		}
	}

	m_bIsDodging = false;
	m_bIsInvincible = false;
	m_bPerfectDodgeWindowActive = false;

	OnDodgeEnd.Broadcast();

	// 상태 복귀
	IActionStateProvider* Provider = GetActionStateProvider();
	if (Provider && Provider->IsActionState(EActionState::DODGING))
	{
		Provider->SetActionState(EActionState::IDLE);
	}
}

void UDodgeComponent::ActivateDodgeEffect()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, m_fPerfectDodgeSlowRate);

	World->GetTimerManager().SetTimer(PerfectDodgeTimerHandle, this, &UDodgeComponent::DeActivateDodgeEffect,
		m_fPerfectDodgeSlowDuration * m_fPerfectDodgeSlowRate, // 실제 시간 기준
		false);

}

void UDodgeComponent::DeActivateDodgeEffect()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
}

IActionStateProvider* UDodgeComponent::GetActionStateProvider() const
{
	return Cast<IActionStateProvider>(GetOwner());
}



