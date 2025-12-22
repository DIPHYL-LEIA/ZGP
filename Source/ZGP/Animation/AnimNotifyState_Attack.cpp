// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_Attack.h"
#include "../CombatAction.h"

void UAnimNotifyState_Attack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (Owner->Implements<UCombatAction>())
	{
		ICombatAction::Execute_BeginAttack(Owner);
	}
}

void UAnimNotifyState_Attack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (Owner->Implements<UCombatAction>())
	{
		ICombatAction::Execute_EndAttack(Owner);
	}
}

FString UAnimNotifyState_Attack::GetNotifyName_Implementation() const
{
	return TEXT("Attack Window");
}
