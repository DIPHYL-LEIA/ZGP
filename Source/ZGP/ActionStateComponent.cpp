// Fill out your copyright notice in the Description page of Project Settings.
#include "ActionStateComponent.h"

UActionStateComponent::UActionStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 생성자에서 설정하여 BeginPlay보다 먼저 InitializeComponent 함수 사용 등록
	// SkillComponent같은 컴포넌트보다 State가 먼저 등록되어 모든 컴포넌트의 기준점이 되어야 함
	// 어떤 BeginPlay가 먼저 실행될지 보장할 수 없기 때문에 '초기화 순서 버그' 방지를 위한 것
	bWantsInitializeComponent = true;
	m_currentState = EActionState::IDLE;
}

void UActionStateComponent::InitializeComponent()
{
	Super::InitializeComponent();
	m_currentState = EActionState::IDLE;
	m_bHitStateCancel = false;
}


void UActionStateComponent::SetActionState(EActionState NewState)
{
	if (m_currentState == NewState) return;
	if (CanChangeActionState(NewState) == false) return;

	const EActionState OldState = m_currentState;

	m_currentState = NewState;

	if (OldState == EActionState::HIT)
	{
		m_bHitStateCancel = false;
	}

	OnStateChanged.Broadcast(OldState, m_currentState);
}

EActionState UActionStateComponent::GetActionState() const
{
	return m_currentState;
}

bool UActionStateComponent::IsActionState(EActionState State) const
{
	return GetActionState() == State;
}

bool UActionStateComponent::CanChangeActionState(EActionState NewState) const
{
	if (m_currentState == EActionState::DEAD) return false;

	if (NewState == EActionState::DEAD) return true;

	// 같은 상태로의 변화는 막되 공격은 허용(연타)
	if (m_currentState == NewState)
	{
		if (NewState == EActionState::ATTACKING)
			return true;
		// 피격 중 다시 피격 허용
		if (NewState == EActionState::HIT)
			return true;
		return false;
	}

	if (m_currentState == EActionState::DAZED && NewState == EActionState::IDLE)
		return true;

	// 공격/회피 후 Idle 상태로 전환 가능 처리
	if (NewState == EActionState::IDLE && IsTemporaryState())
		return true;

	// 피격 중 회피는 bCancel = true 일 때 허용
	if (m_currentState == EActionState::HIT && NewState == EActionState::DODGING)
	{
		return m_bHitStateCancel;
	}

	int32 CurrentPriority = GetStatePriority(m_currentState);
	int32 NewPriority = GetStatePriority(NewState);

	if (NewPriority > CurrentPriority) 
		return true;

	// 예외) 공격 중 회피 캔슬 허용
	if (m_currentState == EActionState::ATTACKING && NewState == EActionState::DODGING)
	{
		return true;
	}

	return false;
}

int32 UActionStateComponent::GetStatePriority(EActionState State) const
{
	switch (State)
	{
	case EActionState::DEAD:
		return 100;
	case EActionState::DAZED:
		return 90;
	case EActionState::HIT:
		return 80;
	case EActionState::DODGING:
		return 70;
	case EActionState::ATTACKING:
		return 50;
	case EActionState::RUNNING:
		return 10;
	case EActionState::WALKING:
		return 10;
	case EActionState::IDLE:
		return 0;
	default:
		return 0;
	}
}

bool UActionStateComponent::IsTemporaryState() const
{
	switch (m_currentState)
	{
	case EActionState::ATTACKING:
	case EActionState::DODGING:
	case EActionState::HIT:
		return true;
	default:
		return false;
	}
}
