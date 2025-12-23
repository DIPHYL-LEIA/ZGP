// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_DodgeWindow.h"
#include "../DodgeComponent.h"

void UAnimNotifyState_DodgeWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UDodgeComponent* DodgeComponent = Owner->FindComponentByClass<UDodgeComponent>())
	{
		DodgeComponent->SetPerfectDodgeWindow(true);
	}
}

void UAnimNotifyState_DodgeWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (UDodgeComponent* DodgeComponent = Owner->FindComponentByClass<UDodgeComponent>())
	{
		DodgeComponent->SetPerfectDodgeWindow(false);
	}
}

FString UAnimNotifyState_DodgeWindow::GetNotifyName_Implementation() const
{
	return TEXT("Perfect Dodge Window");
}
