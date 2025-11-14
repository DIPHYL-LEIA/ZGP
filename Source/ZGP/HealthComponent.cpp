
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	m_fMaxHealth = 100.f;
	m_fCurrentHealth = 0.f;
}

void UHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// ActionState가 DEAD인 상태에서 초기화 함수 실행되면 어떻게 되는 거?
	m_fCurrentHealth = m_fMaxHealth;
}

void UHealthComponent::TakeDamage(float DamageValue, AActor* Character)
{
	if (IsDead() || DamageValue <= 0.f) return;

	const float OldHealth = m_fCurrentHealth;
	m_fCurrentHealth = FMath::Clamp(m_fCurrentHealth - DamageValue, 0.f, m_fMaxHealth);

	OnHealthChanged.Broadcast(Character, m_fCurrentHealth, m_fMaxHealth);

	if (IsDead())
	{
		OnDied.Broadcast(Character);
	}

}

float UHealthComponent::GetCurrentHealth() const
{
	return m_fCurrentHealth;
}

float UHealthComponent::GetMaxHealth() const
{
	return m_fMaxHealth;
}

bool UHealthComponent::IsDead() const
{
	return m_fCurrentHealth <= 0.f;
}


