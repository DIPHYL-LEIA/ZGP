// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ParryImpact.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "../CameraEffectComponent.h"

UAnimNotify_ParryImpact::UAnimNotify_ParryImpact()
{
}

void UAnimNotify_ParryImpact::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (!Character) return;

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC) return;

	UCameraEffectComponent* CameraEffectComponent = PC->FindComponentByClass<UCameraEffectComponent>();
	if (CameraEffectComponent && CameraEffectComponent->IsParrySequenceActive())
	{
		CameraEffectComponent->StartParryImpact();
	}

}

FString UAnimNotify_ParryImpact::GetNotifyName_Implementation() const
{
	return TEXT("Parry Impact");
}
