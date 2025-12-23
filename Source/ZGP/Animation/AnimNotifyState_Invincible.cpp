// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Invincible.h"
#include "../DodgeComponent.h"

void UAnimNotifyState_Invincible::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UDodgeComponent* DodgeComponent = Owner->FindComponentByClass<UDodgeComponent>())
	{
		DodgeComponent->SetInvincible(true);
	}

}

void UAnimNotifyState_Invincible::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UDodgeComponent* DodgeComponent = Owner->FindComponentByClass<UDodgeComponent>())
	{
		DodgeComponent->SetInvincible(false);
	}
}

FString UAnimNotifyState_Invincible::GetNotifyName_Implementation() const
{
	return TEXT("Invincible");
}
