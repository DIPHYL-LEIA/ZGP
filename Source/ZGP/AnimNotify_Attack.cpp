// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Attack.h"
#include "CombatComponent.h"

void UAnimNotify_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UCombatComponent* CombatComponent = Owner->FindComponentByClass<UCombatComponent>();
	if (CombatComponent)
	{
		CombatComponent->PerformAttackTrace(AttackData);
	}

}
