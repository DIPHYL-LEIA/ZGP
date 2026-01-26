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

	float CurrentTime = World->GetRealTimeSeconds();
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
	UE_LOG(LogTemp, Log, TEXT("[Dodge] TryTrigger Called. Window: %d, IsDodging: %d"),
		m_bPerfectDodgeWindowActive, m_bIsDodging);
	if (!m_bPerfectDodgeWindowActive) return false;
	if (!m_bIsDodging) return false;

	UE_LOG(LogTemp, Warning, TEXT("[Dodge] Perfect Dodge TRIGGERED!"));

	//ActivateDodgeEffect();
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

	EDodgeDirection DodgeDirection = CalculateDodgeDirection(Direction);

	if (m_pDodgeMontage)
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UDodgeComponent::HandleMontageEnded);
			AnimInstance->OnMontageEnded.AddDynamic(this, &UDodgeComponent::HandleMontageEnded);
		}
		FName SectionName = GetDodgeSectionName(DodgeDirection);
		Owner->PlayAnimMontage(m_pDodgeMontage, 1.0f, SectionName);
	}
}

EDodgeDirection UDodgeComponent::CalculateDodgeDirection(const FVector& InputDirection) const
{
	if (InputDirection.IsNearlyZero()) 
		return EDodgeDirection::BACKWARD;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return EDodgeDirection::BACKWARD;

	// 카메라/컨트롤러 기준을 캐릭터 로컬 기준으로
	const FVector Forward = Owner->GetActorForwardVector();
	const FVector Right = Owner->GetActorRightVector();

	FVector InputXY = FVector(InputDirection.X, InputDirection.Y, 0.f).GetSafeNormal();
	FVector ForwardXY = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
	FVector RightXY = FVector(Right.X, Right.Y, 0.f).GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(ForwardXY, InputXY);
	const float RightDot = FVector::DotProduct(RightXY, InputXY);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return (ForwardDot > 0.f) ? EDodgeDirection::FORWARD : EDodgeDirection::BACKWARD;
	}
	else
	{
		return (RightDot > 0.f) ? EDodgeDirection::RIGHT : EDodgeDirection::LEFT;
	}
}

FName UDodgeComponent::GetDodgeSectionName(EDodgeDirection Direction)
{
	switch (Direction)
	{
	case EDodgeDirection::FORWARD:
		return FName("Forward");
	case EDodgeDirection::BACKWARD:
		return FName("Backward");
	case EDodgeDirection::LEFT:
		return FName("Left");
	case EDodgeDirection::RIGHT:
		return FName("Right");
	default:
		return FName("Forward");
	}
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

	AActor* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		// Global Time Dilation이 0.1인 경우 Custom Time Dilation을 10으로 적용하여 플레이어 속도 조정
		Owner->CustomTimeDilation = 1.0f / m_fPerfectDodgeSlowRate;
	}
}

void UDodgeComponent::DeActivateDodgeEffect()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);

	AActor* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		Owner->CustomTimeDilation = 1.0f;
	}

}

IActionStateProvider* UDodgeComponent::GetActionStateProvider() const
{
	return Cast<IActionStateProvider>(GetOwner());
}



