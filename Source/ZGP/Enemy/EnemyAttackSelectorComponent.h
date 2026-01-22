// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackData.h"
#include "EnemyAttackSelectorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackSelect, FName, AttackID, const FEnemyAttackData&, AttackcData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackCooldownComplete, FName, AttackID);

/*
* 공격 선택 결과
*/
USTRUCT(BlueprintType)
struct FAttackSelectResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly)
	FName SelectAttackID = NAME_None;

	UPROPERTY(BlueprintReadOnly)
	FEnemyAttackData AttackData;

	FAttackSelectResult() {}
	FAttackSelectResult(bool Success, FName AttackID, const FEnemyAttackData& Data) 
		: bSuccess(Success), SelectAttackID(AttackID), AttackData(Data)
	{}
};

/// <summary>
/// 적 공격 선택 컴포넌트
/// 
/// - 거리, HP, 페이즈 기반 공격 선택
/// - 가중치 랜덤 선택
/// - 쿨다운 관리
/// - 콤보 연계 관리
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZGP_API UEnemyAttackSelectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyAttackSelectorComponent();

	/* 선택 */
	UFUNCTION(BlueprintCallable, Category = "Attack Select")
	FAttackSelectResult SelectAttackDistance(float Distance);

	UFUNCTION(BlueprintCallable, Category = "Attack Select")
	FAttackSelectResult SelectAttackRandom(const TArray<FName>& Attack);

	UFUNCTION(BlueprintCallable, Category = "Attack Select")
	FAttackSelectResult SelectAttackPhase(int32 CurrentPhase, float Distance);

	UFUNCTION(BlueprintCallable, Category = "Attack Select")
	FAttackSelectResult SelectAttackID(FName AttackID);

	UFUNCTION(BlueprintCallable, Category = "Attack Select")
	FAttackSelectResult SelectAttackNextCombo(FName CurrentAttackID);

	/* 쿨다운 */
	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	void StartAttackCooldown(FName AttackID);

	UFUNCTION(BlueprintPure, Category = "Cooldown")
	bool IsAttackReady(FName AttackID) const;

	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetRemainingCooldown(FName AttackID) const;

	UFUNCTION(BlueprintCallable, Category = "Cooldown")
	void ResetAllCooldown();

	/* 데이터 조회 */
	UFUNCTION(BlueprintPure, Category = "Data")
	bool GetAttackData(FName AttackID, FEnemyAttackData& Data) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	bool GetPattern(FEnemyAttackPattern& Pattern) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FName> GetAttackDistance(float Distance) const;

	UFUNCTION(BlueprintPure, Category = "Data")
	TArray<FName> GetAvailableAttack(const TArray<FName>& Attack, float Distance, float HPRatio, int32 Phase) const;

	UFUNCTION(BlueprintPure, Category = "State")
	int32 GetCurrentPhase() const;

	UFUNCTION(BlueprintPure, Category = "State")
	float GetCurrentHPRatio() const;

	/* 델리게이트 */
	UPROPERTY(BlueprintAssignable)
	FOnAttackSelect OnAttackSelect;

	UPROPERTY(BlueprintAssignable)
	FOnAttackCooldownComplete OnAttackCooldownComplete;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UDataTable> m_pAttackDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UDataTable> m_pPatternTable;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	FName m_PatternRowName;

	UPROPERTY(EditDefaultsOnly, Category = "Select")
	EEnemyAttackSelectMode m_eDefaultSelectMode = EEnemyAttackSelectMode::DISTANCE_BASE;

	UPROPERTY(EditDefaultsOnly, Category = "Select")
	bool m_bPreventRepeatAttack = true;

	UPROPERTY(EditDefaultsOnly, Category = "Select", meta = (EditCondition = "!m_bPreventRepeatAttack", ClampMin = "1"))
	int32 m_nMaxRepeatCount = 2;
	
private:
	/* 내부 상태 */
	// 쿨다운 타이머 맵(AttackID -> 쿨다운 종료 시간)
	TMap<FName, float> m_mapCooldownEnd;
	FName m_LastSelectAttackID = NAME_None;
	int32 m_nRepeatCount = 0;
	const FEnemyAttackPattern* m_pCachedPattern = nullptr;

	void CachePattern();
	const FEnemyAttackData* FindAttackData(FName AttackID) const;
	FName SelectAttackWeight(const TArray<FName>& AttackIDs) const;
	bool CanUseAttack(FName AttackID, float Distance, float HPRatio, int32 Phase) const;
	void UpdateCooldown();

	/* HealthComponent에서 HP 가져오기 */
	float SetHPRatio() const;



};
