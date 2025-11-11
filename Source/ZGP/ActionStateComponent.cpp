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
}


void UActionStateComponent::SetActionState(EActionState NewState)
{
	if (m_currentState == NewState) return;
	if (CanChangeActionState(NewState) == false) return;

	const EActionState OldState = m_currentState;

	m_currentState = NewState;

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

	if (NewState == EActionState::ATTACKING)
	{
		switch (m_currentState)
		{
		case EActionState::IDLE:
		case EActionState::WALKING:
		case EActionState::RUNNING:
			return true;
		case EActionState::ATTACKING:
			return true;
		case EActionState::DODGING:
		case EActionState::HIT:
		case EActionState::DAZED:
		default:
			return false;
		}
	}

	if (NewState == EActionState::DODGING)
	{
		switch (m_currentState)
		{
		case EActionState::IDLE:
		case EActionState::WALKING:
		case EActionState::RUNNING:
		case EActionState::ATTACKING:
			return true;
		case EActionState::DODGING:
		case EActionState::HIT:
		case EActionState::DAZED:
		default:
			return false;
		}
	}

	if (NewState == EActionState::HIT && NewState == EActionState::DAZED)
	{
		if (m_currentState == EActionState::DODGING)
		{
			// 회피 중(무적) 피격/그로기 상태 될 수 없음
			return false;
		}
		return true;
	}

	return true;
}
