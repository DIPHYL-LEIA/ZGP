// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillData.h"
#include "SkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestPlayMontage, UAnimMontage*, MontageToPlay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillExecuteCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeavyAttackHit, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, float, DeltaEnergy, float, DeltaDecibel);

/// <summary>
/// 스킬 컴포넌트		: 스킬 / 콤보 실행 담당
/// 
/// FSkillData 조회
/// 몽타주 재생 요청
/// 스킬 데이터 캐싱
/// 
/// </summary>

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZGP_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> m_pSkillDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> m_pComboDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData", meta = (AllowPrivateAccess = "true"))
	FName m_ComboDataRowName;

	UPROPERTY(VisibleInstanceOnly, Category = "Skill")
	FName m_CurrentSkillID;

	const FSkillData* m_pCurrentSkillData = nullptr;
	bool m_bIsCurrentLastCombo = false;

public:
	// Bind ComboComponent
	UFUNCTION()
	void ExecuteComboAttack(int32 ComboIndex, bool bIsLastcombo);
	
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool ExecuteSkillID(FName SkillID);

	// 스킬 실행 여부 확인 (자원 체크)
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool CanExecuteSkill(FName SkillID) const;

	// Heavy Attack
	UFUNCTION(BlueprintPure, Category = "Skill")
	bool IsCurrentSkillHeavy() const;

	// Stunned 상태의 적에게 Heavy Attack 적중 시 Chain Attack 트리거
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void NotifyHeavyAttackHit(AActor* Target);

	// 상태
	UFUNCTION(BlueprintPure, Category = "Skill")
	FName GetCurrentSkillID() const { return m_CurrentSkillID; }

	const FSkillData* GetCurrentSkillData() const { return m_pCurrentSkillData; }

	UFUNCTION(BlueprintPure, Category = "Skill")
	bool GetCurrentSkillDataBP(FSkillData& Data) const;

	UFUNCTION(BlueprintPure, Category = "Skill")
	ESkillType GetCurrentSkillType() const;

	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnRequestPlayMontage OnRequestPlayMontage;

	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnSkillExecuteCompleted OnSkillExecuteCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnResourceChanged OnResourceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Skill")
	FOnHeavyAttackHit OnHeavyAttackHit;

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void NotifySkillCompleted();

private:
	/// <summary>
	/// 내부 스킬 실행
	/// </summary>
	/// <param name="SkillID"></param>
	/// <param name="bOverrideHeavy"> Heavy Attack 오버라이드? </param>
	/// <param name="bIsHeavyOverride"> 오버라이드 시 Heavy Attack 값 </param>
	/// <returns></returns>
	bool ExecuteSkillInternal(FName SkillID, bool bOverrideHeavy, bool bIsHeavyOverride = false);

	FName GetSkillIDFromComboIndex(int32 ComboIndex) const;

	const FSkillData* FindSkillData(FName SkillID) const;

};
