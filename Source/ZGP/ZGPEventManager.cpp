// Fill out your copyright notice in the Description page of Project Settings.


#include "ZGPEventManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogEventManager, Log, All);
void UZGPEventManager::BroadCast(FGameplayTag EventTag, const FGameplayEventContext& Context)
{
	if (!EventTag.IsValid()) return;

	FString Name;
	if (Context.Instigator.IsValid())
	{
		Name = Context.Instigator.Get()->GetName();
	}
	else
	{
		Name = TEXT("None");
	}
	UE_LOG(LogEventManager, Verbose, TEXT("Broadcasting Event: [%s] by Instigator: [%s]"), *EventTag.ToString(), *Name);
	
	if (FOnGameplayEvent* Dispatcher = EventDispatchers.Find(EventTag))
	{
		if (Dispatcher->IsBound())
		{
			Dispatcher->Broadcast(Context);
		}
	}

}

FOnGameplayEvent& UZGPEventManager::GetEventDelegate(FGameplayTag EventTag)
{
	return EventDispatchers.FindOrAdd(EventTag);
}
