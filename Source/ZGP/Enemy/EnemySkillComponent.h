// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackData.h"
#include "EnemySkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyRequestPlayMontage, UAnimMontage*, MontageToPlay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyAttackCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyAttackStarted, FName, AttackID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCooldownRequest, FName, AttackID);

/**
 * Enemy 공격 실행 컴포넌트
 *
 * 역할:
 * - FEnemyAttackData 기반 공격 실행
 * - 몽타주 재생 요청 (OnRequestPlayMontage)
 * - Heavy Attack 여부 판단 (Chain Attack 트리거용)
 * - 공격 완료 알림 (BTTask_ExecuteAttack 대기 해제)
 *
 * EnemyAttackSelectorComponent와의 관계:
 * - Selector: 어떤 공격을 할지 선택 (조건, 가중치, 쿨다운)
 * - SkillComponent: 선택된 공격을 실행 (몽타주, Heavy 판단)
 *
 * ZZZ 시스템:
 * - Enemy의 Heavy Attack이 Stunned 상태 적에게 적중 시 Chain Attack 트리거
 * - FEnemyAttackData.bIsHeavyAttack으로 판단
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API UEnemySkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemySkillComponent();

protected:
	virtual void BeginPlay() override;

	// ========================================
	// 데이터 테이블
	// ========================================

	/**
	 * 공격 데이터 테이블 (FEnemyAttackData)
	 * Enemy BP별로 다르게 설정 (각 Enemy 종류별 몽타주)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack Data")
	TObjectPtr<UDataTable> m_pAttackDataTable;

	// ========================================
	// 현재 상태
	// ========================================

	UPROPERTY(VisibleInstanceOnly, Category = "State")
	FName m_CurrentAttackID;

	/** 현재 실행 중인 공격 데이터 (캐싱) */
	const FEnemyAttackData* m_pCurrentAttackData = nullptr;

	/** 공격 실행 중 여부 */
	bool m_bIsExecuting = false;

public:
	// ========================================
	// 공격 실행
	// ========================================

	/**
	 * 공격 실행
	 * @param AttackID - FEnemyAttackData의 RowName
	 * @return 실행 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	bool ExecuteAttack(FName AttackID);

	/**
	 * 공격 실행 가능 여부 확인
	 * (DataTable에 존재하고 몽타주가 유효한지)
	 */
	UFUNCTION(BlueprintPure, Category = "Attack")
	bool CanExecuteAttack(FName AttackID) const;

	// ========================================
	// 상태 조회
	// ========================================

	/** 현재 공격 실행 중인지 */
	UFUNCTION(BlueprintPure, Category = "Attack")
	bool IsExecutingAttack() const { return m_bIsExecuting; }

	/** 현재 공격이 Heavy Attack인지 (Chain Attack 트리거용) */
	UFUNCTION(BlueprintPure, Category = "Attack")
	bool IsCurrentAttackHeavy() const;

	/** 현재 실행 중인 공격 ID */
	UFUNCTION(BlueprintPure, Category = "Attack")
	FName GetCurrentAttackID() const { return m_CurrentAttackID; }

	/** 현재 공격 데이터 (Blueprint용) */
	UFUNCTION(BlueprintPure, Category = "Attack")
	bool GetCurrentAttackData(FEnemyAttackData& OutData) const;

	/** 현재 공격의 Flash Type */
	UFUNCTION(BlueprintPure, Category = "Attack")
	EAttackFlashType GetCurrentFlashType() const;

	// ========================================
	// 완료 알림
	// ========================================

	/**
	 * 공격 완료 알림 (몽타주 종료 시 호출)
	 * EnemyCharacter::HandleMontageEnded에서 ISkillExecutor를 통해 호출
	 */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void NotifyAttackCompleted();

	// ========================================
	// 델리게이트
	// ========================================

	/** 몽타주 재생 요청 - EnemyCharacter가 구독 */
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnEnemyRequestPlayMontage OnRequestPlayMontage;

	/** 공격 완료 - BTTask_ExecuteAttack이 대기 해제용 */
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnEnemyAttackCompleted OnAttackCompleted;

	/** 공격 시작 - EnemyFlashComponent 등이 구독 */
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnEnemyAttackStarted OnAttackStarted;

	/** 쿨다운 요청 - EnemyCharacter에서 중개 */
	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnCooldownRequest OnCooldownRequest;

private:
	/**
	 * 공격 데이터 조회
	 * @param AttackID - RowName
	 * @return 데이터 포인터 (없으면 nullptr)
	 */
	const FEnemyAttackData* FindAttackData(FName AttackID) const;

	/**
	 * 쿨다운 시작 알림
	 * EnemyCharacter에서 중개하여
	 * EnemyAttackSelectorComponent에 전달
	 */
	void NotifyCooldownStart(FName AttackID);
};