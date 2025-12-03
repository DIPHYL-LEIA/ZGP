// Fill out your copyright notice in the Description page of Project Settings.


#include "TokenManagerComponent.h"

UTokenManagerComponent::UTokenManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	m_nMaxTargetAttacker = 2;
}

bool UTokenManagerComponent::RequestToken(AActor* Attacker, ETokenPriority Priority)
{
	if (!Attacker) return false;

	CleanToken();

	if (HasToken(Attacker)) 
		return true;

	// 보스는 토큰 제한 무시
	if (Priority == ETokenPriority::BOSS)
	{
		m_arTokenHolders.Add(Attacker);
		return true;
	}

	if (m_arTokenHolders.Num() >= m_nMaxTargetAttacker) 
		return false;

	m_arTokenHolders.Add(Attacker);
	
	return true;
}

void UTokenManagerComponent::ReleaseToken(AActor* Attacker)
{
	if (!Attacker) return;

	for (int32 i = m_arTokenHolders.Num() - 1; i >= 0; --i)
	{
		if (m_arTokenHolders[i].Get() == Attacker)
		{
			m_arTokenHolders.RemoveAt(i);
			return;
		}
	}
}

bool UTokenManagerComponent::HasToken(AActor* Attacker) const
{
	if (!Attacker) return false;

	for (const TWeakObjectPtr<AActor>& Holder : m_arTokenHolders)
	{
		if (Holder.IsValid() && Holder.Get() == Attacker)
		{
			return true;
		}
	}
	return false;
}

int32 UTokenManagerComponent::GetCurrentAttackerCount() const
{
	int32 ValidCount = 0;
	for (const TWeakObjectPtr<AActor>& Holder : m_arTokenHolders)
	{
		if (Holder.IsValid())
		{
			++ValidCount;
		}
	}
	return ValidCount;
}

void UTokenManagerComponent::CleanToken()
{
	for (int32 i = m_arTokenHolders.Num() - 1; i >= 0; --i)
	{
		if (!m_arTokenHolders[i].IsValid())
		{
			m_arTokenHolders.RemoveAt(i);
		}
	}
}
