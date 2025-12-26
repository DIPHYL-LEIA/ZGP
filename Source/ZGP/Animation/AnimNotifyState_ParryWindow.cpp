// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_ParryWindow.h"
#include "../ParryDetectorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UAnimNotifyState_ParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UParryDetectorComponent* ParryComponent = GetParryDetector(MeshComp);
	if (ParryComponent)
	{
		ParryComponent->RegisterParryableAttack(Owner);
	}

}

void UAnimNotifyState_ParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UParryDetectorComponent* ParryComponent = GetParryDetector(MeshComp);
	if (ParryComponent)
	{
		ParryComponent->UnregisterParryableAttack(Owner);
	}
}

FString UAnimNotifyState_ParryWindow::GetNotifyName_Implementation() const
{
	return TEXT("Parry Window");
}

UParryDetectorComponent* UAnimNotifyState_ParryWindow::GetParryDetector(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp) return nullptr;

	UWorld* World = MeshComp->GetWorld();
	if (!World) return nullptr;

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC) return nullptr;

	return PC->FindComponentByClass<UParryDetectorComponent>();
}
