// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EnemyFlash.h"
#include "../Enemy/EnemyFlashComponent.h"

void UAnimNotify_EnemyFlash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UEnemyFlashComponent* FlashComp = Owner->FindComponentByClass<UEnemyFlashComponent>();
	//if (FlashComp)
	//{
	//	FlashComp->StartFlash(FlashType);
	//}

	// È®ÀÎ¿ë
	if (FlashComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AnimNotify] EnemyFlash - Type: %d, Owner: %s"),
			static_cast<int32>(FlashType), *Owner->GetName());
		FlashComp->StartFlash(FlashType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AnimNotify] EnemyFlash - No FlashComponent on %s"),
			*Owner->GetName());
	}
}

FString UAnimNotify_EnemyFlash::GetNotifyName_Implementation() const
{
	return TEXT("Enemy Flash");
}
