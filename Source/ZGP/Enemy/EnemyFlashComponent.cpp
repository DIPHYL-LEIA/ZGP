// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlashComponent.h"
#include "EnemySkillComponent.h"
#include "EnemyAttackSelectorComponent.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


UEnemyFlashComponent::UEnemyFlashComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyFlashComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (m_bAutoBindSkillComponent)
	{
		BindSkillComponent();
	}
}

void UEnemyFlashComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_FlashTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_FlashTimerHandle);
	}

	CleanNiagaraEffect();

	Super::EndPlay(EndPlayReason);
}

void UEnemyFlashComponent::StartFlash(EAttackFlashType FlashType, FName AttackID, float Duration)
{
	if (FlashType == EAttackFlashType::NONE || FlashType == EAttackFlashType::MAX) return;

	// 기존 Flash 정리
	if (m_bIsFlashing)
	{
		StopFlash();
	}

	m_bIsFlashing = true;
	m_eCurrentFlashType = FlashType;
	m_CurrentAttackID = AttackID;

	SpawnNiagaraEffect(FlashType);
	PlayFlashSound(FlashType);

	OnFlashStart.Broadcast(FlashType, AttackID);

	if (m_bUseAutoStop)
	{
		float ActualDuration = (Duration > 0.f) ? Duration : m_fDefaultFlashDuration;

		GetWorld()->GetTimerManager().SetTimer(m_FlashTimerHandle, this, &UEnemyFlashComponent::OnFlashTimeExpired, ActualDuration, false);
	}
	UE_LOG(LogTemp, Log, TEXT("[EnemyFlashComponent] Flash Started - Type: %s, Attack: %s"),
		FlashType == EAttackFlashType::GOLD ? TEXT("GOLD") : TEXT("RED"),
		*AttackID.ToString());
}

void UEnemyFlashComponent::StopFlash()
{
	if (!m_bIsFlashing) return;

	if (m_FlashTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_FlashTimerHandle);
	}
	
	CleanNiagaraEffect();

	EAttackFlashType PrevType = m_eCurrentFlashType;
	m_bIsFlashing = false;
	m_eCurrentFlashType = EAttackFlashType::NONE;
	m_CurrentAttackID = NAME_None;

	OnFlashEnd.Broadcast();
}

void UEnemyFlashComponent::SpawnNiagaraEffect(EAttackFlashType FlashType)
{
	CleanNiagaraEffect();

	UNiagaraSystem* EffectSystem = nullptr;
	switch (FlashType)
	{
	case EAttackFlashType::GOLD:
		EffectSystem = m_pGoldFlashSystem;
		break;
	case EAttackFlashType::RED:
		EffectSystem = m_pRedFlashSystem;
		break;
	default:
		return;
	}

	if (!EffectSystem) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 부착 위치
	USceneComponent* Attach = Owner->GetRootComponent();
	FVector SpawnLocation = Owner->GetActorLocation() + m_EffectOffset;

	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			Attach = Mesh;

			if (!m_EffectAttachSocket.IsNone() && Mesh->DoesSocketExist(m_EffectAttachSocket))
			{
				SpawnLocation = Mesh->GetSocketLocation(m_EffectAttachSocket) + m_EffectOffset;
			}
		}
	}

	// 나이아가라 컴포넌트 생성
	m_pNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EffectSystem, SpawnLocation, FRotator::ZeroRotator, m_EffectScale,
		true, true, ENCPoolMethod::None, true);

	// 소켓 부착 (선택적)
	if (m_pNiagaraComponent && Attach && !m_EffectAttachSocket.IsNone())
	{
		m_pNiagaraComponent->AttachToComponent(
			Attach,
			FAttachmentTransformRules::KeepWorldTransform,
			m_EffectAttachSocket
		);
	}
}

void UEnemyFlashComponent::CleanNiagaraEffect()
{
	if (m_pNiagaraComponent)
	{
		m_pNiagaraComponent->DestroyComponent();
		m_pNiagaraComponent = nullptr;
	}
}

void UEnemyFlashComponent::PlayFlashSound(EAttackFlashType FlashType)
{
	USoundBase* SoundPlay = nullptr;

	switch (FlashType)
	{
	case EAttackFlashType::GOLD:
		SoundPlay = m_pGoldFlashSound;
		break;
	case EAttackFlashType::RED:
		SoundPlay = m_pRedFlashSound;
		break;
	default:
		return;
	}

	if (!SoundPlay) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundPlay, Owner->GetActorLocation(), m_fSoundVolumeMultiplier);
}

void UEnemyFlashComponent::OnFlashTimeExpired()
{
	StopFlash();
}

void UEnemyFlashComponent::BindSkillComponent()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UEnemySkillComponent* EnemySkillComponent = Owner->FindComponentByClass<UEnemySkillComponent>();
	if (EnemySkillComponent)
	{
		EnemySkillComponent->OnAttackStarted.AddDynamic(this, &UEnemyFlashComponent::HandleAttackStarted);
	}
}

void UEnemyFlashComponent::HandleAttackStarted(FName AttackID)
{
	EAttackFlashType FlashType = GetFlashTypeAttack(AttackID);

	if (FlashType != EAttackFlashType::NONE)
	{
		StartFlash(FlashType, AttackID);
	}
}

EAttackFlashType UEnemyFlashComponent::GetFlashTypeAttack(FName AttackID) const
{
	AActor* Owner = GetOwner();
	if (!Owner) return EAttackFlashType::NONE;

	UEnemyAttackSelectorComponent* Selector = Owner->GetComponentByClass<UEnemyAttackSelectorComponent>();
	if (!Selector) return EAttackFlashType::NONE;

	FEnemyAttackData AttackData;
	if (Selector->GetAttackData(AttackID, AttackData))
	{
		return AttackData.FlashType;
	}
	return EAttackFlashType::NONE;
}
