// Fill out your copyright notice in the Description page of Project Settings.


#include "HitReactionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UHitReactionComponent::UHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHitReactionComponent::RunHitReaction(const FDamageData& DamageData)
{
	if (DamageData.HitReaction == EHitReactionType::NONE) return;

	EHitDirection Direction = CalculateHitDirection(DamageData.HitDirection);

	OnHitReactionStart.Broadcast(DamageData.HitReaction);

	ApplyKnockback(DamageData.HitDirection, DamageData.HitReaction);
	PlayHitMontage(DamageData.HitReaction, Direction);
}

EHitDirection UHitReactionComponent::CalculateHitDirection(const FVector& HitDirection) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return EHitDirection::FRONT;

	const FVector Forward = Owner->GetActorForwardVector();
	const FVector Right = Owner->GetActorRightVector();

	FVector HitDirectionXY = FVector(HitDirection.X, HitDirection.Y, 0.f).GetSafeNormal();
	FVector ForwardXY = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
	FVector RightXY = FVector(Right.X, Right.Y, 0.f).GetSafeNormal();

	const float ForwardDot = FVector::DotProduct(ForwardXY, HitDirectionXY);
	const float RightDot = FVector::DotProduct(RightXY, HitDirectionXY);

	if (FMath::Abs(ForwardDot) >= FMath::Abs(RightDot))
	{
		return (ForwardDot > 0.f) ? EHitDirection::BACK : EHitDirection::FRONT;
	}
	else
	{
		return (RightDot > 0.f) ? EHitDirection::LEFT : EHitDirection::RIGHT;
	}
}

void UHitReactionComponent::PlayHitMontage(EHitReactionType ReactionType, EHitDirection Direction)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	UAnimMontage* Montage = nullptr;

	switch (ReactionType)
	{
	case EHitReactionType::LIGHT:
	case EHitReactionType::MEDIUM:
		Montage = m_pHitLightMontage;
		break;
	case EHitReactionType::HEAVY:
	case EHitReactionType::LAUNCH:
		Montage = m_pHitHeavyMontage;
		break;
	case EHitReactionType::KNOCKDOWN:
		Montage = m_pKnockdownMontage;
		break;
	default:
		return;
	}

	if (Montage)
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHitReactionComponent::HandleMontageEnd);
			AnimInstance->OnMontageEnded.AddDynamic(this, &UHitReactionComponent::HandleMontageEnd);
		}
		FName Name = GetSectionName(Direction);
		Owner->PlayAnimMontage(Montage, 1.0f, Name);
	}

}

void UHitReactionComponent::ApplyKnockback(const FVector& HitDirection, EHitReactionType ReactionType)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (!Owner) return;

	UCharacterMovementComponent* MovementComponent = Owner->GetCharacterMovement();
	if (!MovementComponent) return;

	float Strength = 0.f;

	switch (ReactionType)
	{
	case EHitReactionType::LIGHT:
		Strength = m_fLightKnockback;
		break;
	case EHitReactionType::MEDIUM:
	case EHitReactionType::HEAVY:
	case EHitReactionType::LAUNCH:
	case EHitReactionType::KNOCKDOWN:
		Strength = m_fHeavyKnockback;
		break;
	default:
		break;
	}

	FVector KnockbackDirection = FVector(HitDirection.X, HitDirection.Y, 0.f).GetSafeNormal();
	if (KnockbackDirection.IsNearlyZero()) return;

	FVector LaunchVelocity = KnockbackDirection * Strength;

	if (ReactionType == EHitReactionType::LAUNCH)
	{
		LaunchVelocity.Z = Strength * m_fLaunchVerticalRatio;
	}

	MovementComponent->StopMovementImmediately();
	Owner->LaunchCharacter(LaunchVelocity, true, true);

}

FName UHitReactionComponent::GetSectionName(EHitDirection Direction)
{
	switch (Direction)
	{
	case EHitDirection::FRONT:
		return FName("Front");
	case EHitDirection::BACK:
		return FName("Back");
	case EHitDirection::LEFT:
		return FName("Left");
	case EHitDirection::RIGHT:
		return FName("Right");
	default:
		return FName("Front");
	}
}

void UHitReactionComponent::HandleMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		if (UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.RemoveDynamic(this, &UHitReactionComponent::HandleMontageEnd);
		}
	}
	OnHitReactionEnd.Broadcast();
}
