// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RequestToken.h"

UBTTask_RequestToken::UBTTask_RequestToken()
{
}

EBTNodeResult::Type UBTTask_RequestToken::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Type();
}

FString UBTTask_RequestToken::GetStaticDescription() const
{
	return FString();
}
