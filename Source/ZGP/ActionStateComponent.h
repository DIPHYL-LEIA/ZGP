// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionState.h"
#include "ActionStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionStateChanged, EActionState, OldState, EActionState, NewState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UActionStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActionStateComponent();

	// 컴포넌트 초기화 시 호출
	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "ActionState")
	void SetActionState(EActionState NewState);

	UFUNCTION(BlueprintPure, Category = "ActionState")
	EActionState GetActionState() const;

	UFUNCTION(BlueprintPure, Category = "ActionState")
	bool IsActionState(EActionState State) const;

	UFUNCTION(BlueprintPure, Category = "ActionState")
	bool CanChangeActionState(EActionState NewState) const;

	UPROPERTY(BlueprintAssignable, Category = "ActionState")
	FOnActionStateChanged OnStateChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "ActionState", meta = (AllowPrivateAccess = "true"))
	EActionState m_currentState;

	// 멀티 플레이
	//UFUNCTION()
	//void OnRep_CurrentState(EActionState OldState);

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
