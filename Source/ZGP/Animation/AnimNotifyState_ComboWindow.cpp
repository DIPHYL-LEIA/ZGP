// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ComboWindow.h"
#include "../ComboComponent.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UComboComponent* ComboComponent = Owner->FindComponentByClass<UComboComponent>();
	if (ComboComponent)
	{
		ComboComponent->StartComboWindow();
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UComboComponent* ComboComponent = Owner->FindComponentByClass<UComboComponent>();
	if (ComboComponent)
	{
		ComboComponent->EndComboWindow();
	}
}

FString UAnimNotifyState_ComboWindow::GetNotifyName_Implementation() const
{
	return TEXT("Combo Window");
}
