// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../BaseCharacter.h"

const FName AEnemyAIController::BB_TargetActor	= FName("TargetActor");
const FName AEnemyAIController::BB_AIState		= FName("AIState");
const FName AEnemyAIController::BB_CanAct		= FName("CanAct");

AEnemyAIController::AEnemyAIController()
{
}

EEnemyAIState AEnemyAIController::GetCurrentAIState() const
{
	return EEnemyAIState::NONE;
}
