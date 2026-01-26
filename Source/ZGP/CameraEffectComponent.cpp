// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraEffectComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

UCameraEffectComponent::UCameraEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Perfect Dodge
	m_PerfectDodgeParams.bEnableSlowMotion = true;
	m_PerfectDodgeParams.SlowMotionScale = 0.1f;
	m_PerfectDodgeParams.bEnableZoom = true;
	m_PerfectDodgeParams.ZoomFOVOffset = -10.f;
	m_PerfectDodgeParams.bFocusTarget = true;
	m_PerfectDodgeParams.bEnableShake = true;
	m_PerfectDodgeParams.Duration = 0.8f;

	// Parry Assist
	m_ParryAssistParams.bEnableSlowMotion = true;
	m_ParryAssistParams.SlowMotionScale = 0.3f;
	m_ParryAssistParams.bEnableShake = true;
	m_ParryAssistParams.ShakeScale = 0.5f;
	m_ParryAssistParams.bFocusTarget = false;
	m_ParryAssistParams.bExcludePlayer = true;
	m_ParryAssistParams.Duration = 0.3f;

	// Chain Attack
	m_ChainAttackParams.bEnableSlowMotion = true;
	m_ChainAttackParams.bEnableZoom = true;
	m_ChainAttackParams.ZoomFOVOffset = -20.f;
	m_ChainAttackParams.bFocusTarget = true;
	m_ChainAttackParams.Duration = 0.f;			// 수동 종료
}

void UCameraEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetPlayerController();
	if (PC && PC->PlayerCameraManager)
	{
		m_fOriginalFOV = PC->PlayerCameraManager->GetFOVAngle();
		m_fCurrentFOV = m_fOriginalFOV;
		m_fTargetFOV = m_fOriginalFOV;
	}
}

void UCameraEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bIsZoom)
	{
		UpdateZoom(DeltaTime);
	}

	if (!m_bIsPlay) return;

	// Duration 0이면 수동 종료
	if (m_fEffectDuration > 0.f)
	{
		m_fEffectTimer += DeltaTime;

		if (m_fEffectTimer >= m_fEffectDuration)
		{
			EndEffect();
		}
	}
}

void UCameraEffectComponent::PlayEffect(ECameraEffectType EffectType, AActor* FocusTarget)
{
	const FCameraEffectParams* Params = GetParams(EffectType);
	if (!Params) return;

	m_eCurrentEffectType = EffectType;
	StartEffect(*Params, FocusTarget);
}

void UCameraEffectComponent::PlayCustomEffect(const FCameraEffectParams& Params, AActor* FocusTarget)
{
	m_eCurrentEffectType = ECameraEffectType::CUSTOM;
	StartEffect(Params, FocusTarget);
}

void UCameraEffectComponent::StopEffect()
{
	if (m_bIsPlay)
	{
		EndEffect();
	}
}

void UCameraEffectComponent::StartEffect(const FCameraEffectParams& Params, AActor* FocusTarget)
{
	if (m_bIsPlay)
	{
		EndEffect();
	}

	m_bIsPlay = true;
	m_CurrentParams = Params;
	m_pFocusTarget = FocusTarget;
	m_fEffectTimer = 0.f;
	m_fEffectDuration = Params.Duration;

	if (Params.bEnableSlowMotion)
	{
		ApplySlowMotion(Params.SlowMotionScale, Params.bExcludePlayer);
	}

	if (Params.bEnableZoom)
	{
		ApplyZoom(Params.ZoomFOVOffset);
	}

	if (Params.bEnableShake && Params.ShakeClass)
	{
		ApplyShake(Params.ShakeClass, Params.ShakeScale);
	}

	if (Params.bFocusTarget && FocusTarget)
	{
		ApplyFocus(FocusTarget);
	}

	SetComponentTickEnabled(true);

	OnCameraEffectStart.Broadcast(m_eCurrentEffectType);

	UE_LOG(LogTemp, Log, TEXT("[CameraDirector] Effect Started: %d"), static_cast<int32>(m_eCurrentEffectType));
}

void UCameraEffectComponent::EndEffect()
{
	if (!m_bIsPlay) return;

	if (m_CurrentParams.bEnableSlowMotion)
	{
		RestoreSlowMotion();
	}
	if (m_CurrentParams.bEnableZoom)
	{
		RestoreZoom();
	}
	if (m_CurrentParams.bFocusTarget)
	{
		RestoreFocus();
	}

	ECameraEffectType EndType = m_eCurrentEffectType;

	m_bIsPlay = false;
	m_eCurrentEffectType = ECameraEffectType::NONE;
	m_pFocusTarget.Reset();
	m_fEffectTimer = 0.f;

	// 보간 중이면 Tick 유지
	if (!m_bIsZoom)
	{
		SetComponentTickEnabled(false);
	}

	OnCameraEffectEnd.Broadcast(EndType);

	UE_LOG(LogTemp, Log, TEXT("[CameraDirector] Effect Ended: %d"), static_cast<int32>(m_eCurrentEffectType));
}

void UCameraEffectComponent::ApplySlowMotion(float Scale, bool bExcludePlayer)
{
	UWorld* World = GetWorld();
	if (!World) return;

	m_fOriginalTimeDelay = UGameplayStatics::GetGlobalTimeDilation(World);
	m_bSlowMotionActive = true;
	m_bExcludePlayerFromSlow = bExcludePlayer;

	UGameplayStatics::SetGlobalTimeDilation(World, Scale);

	if (bExcludePlayer)
	{
		APawn* Pawn = GetActivePawn();
		if (Pawn)
		{
			Pawn->CustomTimeDilation = 1.0f / Scale;
			m_pExcludedPawn = Pawn;
		}
	}
}

void UCameraEffectComponent::RestoreSlowMotion()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::SetGlobalTimeDilation(World, m_fOriginalTimeDelay);

	if (m_pExcludedPawn.IsValid())
	{
		m_pExcludedPawn->CustomTimeDilation = 1.0f;
		m_pExcludedPawn.Reset();
	}

	// 플레이어 복원
	APawn* Pawn = GetActivePawn();
	if (Pawn)
	{
		Pawn->CustomTimeDilation = 1.0f;
	}
	m_bSlowMotionActive = false;
}

void UCameraEffectComponent::ExcludeCurrentPawn()
{
	if (!m_bSlowMotionActive || !m_bExcludePlayerFromSlow) return;

	APawn* Pawn = GetActivePawn();
	if (Pawn)
	{
		float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
		if (CurrentDilation > 0.f && CurrentDilation < 1.f)
		{
			Pawn->CustomTimeDilation = 1.0f / CurrentDilation;
		}
	}
}

void UCameraEffectComponent::StartParrySequence(AActor* ParriedEnemy)
{
	if (m_bIsPlay)
	{
		EndEffect();
	}
	if (m_bParrySequenceActive)
	{
		EndParrySequence();
	}

	m_bParrySequenceActive = true;
	m_pParriedEnemy = ParriedEnemy;
}

void UCameraEffectComponent::StartParryImpact()
{
	if (!m_bParrySequenceActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraEffect] TriggerParryImpact - Sequence not active"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	m_fOriginalTimeDelay = UGameplayStatics::GetGlobalTimeDilation(World);
	UGameplayStatics::SetGlobalTimeDilation(World, m_ParrySequenceParams.ImpactTimeScale);
	m_bSlowMotionActive = true;

	// 줌 인
	ApplyZoom(m_ParrySequenceParams.ImpactZoomFOV);

	if (m_ParrySequenceParams.ImpactShakeClass)
	{
		APlayerController* PC = GetPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(m_ParrySequenceParams.ImpactShakeClass, m_ParrySequenceParams.ShakeScale);
		}
	}

	// 실제 시간 기준 타이머
	// 게임 시간 = 실제 시간 * TimeScale
	float GameTimeDuration = m_ParrySequenceParams.ImpactRealDuration * m_ParrySequenceParams.ImpactTimeScale;

	World->GetTimerManager().ClearTimer(m_ParryImpactTimerHandle);
	World->GetTimerManager().SetTimer(m_ParryImpactTimerHandle, this, &UCameraEffectComponent::EndParryImpact, GameTimeDuration, false);

}

void UCameraEffectComponent::EndParrySequence()
{
	if (!m_bParrySequenceActive) return;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(m_ParryImpactTimerHandle);

		// 혹시 슬로우 상태면 복원
		if (m_bSlowMotionActive)
		{
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			m_bSlowMotionActive = false;
		}
	}

	// 줌 복원
	if (m_bIsZoom)
	{
		RestoreZoom();
	}

	m_bParrySequenceActive = false;
	m_pParriedEnemy.Reset();

	UE_LOG(LogTemp, Warning, TEXT("[CameraEffect] Parry Sequence Ended"));
}

void UCameraEffectComponent::EndParryImpact()
{
	if (!m_bParrySequenceActive) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 반격은 즉시 정상 속도
	UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	m_bSlowMotionActive = false;

	RestoreZoom();
}


void UCameraEffectComponent::ApplyZoom(float FOVOffset)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;

	// 원본 저장 (처음 한 번만)
	if (!m_bIsZoom)
	{
		m_fOriginalFOV = PC->PlayerCameraManager->GetFOVAngle();
		m_fCurrentFOV = m_fOriginalFOV;
	}

	// 목표 FOV 설정
	m_fTargetFOV = m_fOriginalFOV + FOVOffset;
	m_fTargetFOV = FMath::Clamp(m_fTargetFOV, 30.f, 120.f);

	m_bIsZoom = true;
	SetComponentTickEnabled(true);
}

void UCameraEffectComponent::RestoreZoom()
{
	// 원본 FOV로 목표 설정 (보간으로 복원)
	m_fTargetFOV = m_fOriginalFOV;
	m_bIsZoom = true;
	SetComponentTickEnabled(true);
}

void UCameraEffectComponent::ApplyShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;

	PC->PlayerCameraManager->StartCameraShake(ShakeClass, Scale);
}

void UCameraEffectComponent::ApplyFocus(AActor* Target)
{
}

void UCameraEffectComponent::RestoreFocus()
{
}

const FCameraEffectParams* UCameraEffectComponent::GetParams(ECameraEffectType EffectType) const
{
	switch (EffectType)
	{
	case ECameraEffectType::PERPECT_DODGE:
		return &m_PerfectDodgeParams;
	case ECameraEffectType::PARRY_ASSIST:
		return &m_ParryAssistParams;
	case ECameraEffectType::CHAIN_ATTACK:
		return &m_ChainAttackParams;
	default:
		return nullptr;
	}
}

APlayerController* UCameraEffectComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

APawn* UCameraEffectComponent::GetActivePawn() const
{
	APlayerController* PC = GetPlayerController();
	return PC ? PC->GetPawn() : nullptr;
}

void UCameraEffectComponent::UpdateZoom(float DeltaTime)
{
	APlayerController* PC = GetPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;

	m_fCurrentFOV = FMath::FInterpTo(m_fCurrentFOV, m_fTargetFOV, DeltaTime, m_fZoomInterpSpeed);

	PC->PlayerCameraManager->SetFOV(m_fCurrentFOV);

	if (FMath::IsNearlyEqual(m_fCurrentFOV, m_fTargetFOV, 0.1f))
	{
		m_fCurrentFOV = m_fTargetFOV;
		PC->PlayerCameraManager->SetFOV(m_fCurrentFOV);

		// 원본으로 복원 완료 시에만 Zoom 종료
		if (FMath::IsNearlyEqual(m_fTargetFOV, m_fOriginalFOV, 0.1f))
		{
			m_bIsZoom = false;
		}
	}
}



