// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_AirMotion.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotifyState_AirMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());

	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void UAnimNotifyState_AirMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());

	if (Character && Character->GetCharacterMovement())
	{
		// 몽타주가 끝나거나 구간이 끝나면 다시 중력 적용
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}

FString UAnimNotifyState_AirMotion::GetNotifyName_Implementation() const
{
	return TEXT("Air Motion");
}
