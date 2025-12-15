// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTTask_Base.h"
#include "BTTask_Moving.generated.h"

UENUM(BlueprintType)
enum class EMoveType : uint8
{
	MOVE_AWAY		UMETA(DisplayName = "Move Away"),
	STRAFE_LEFT		UMETA(DisplayName = "Strafe Left"),
	STRAFE_RIGHT	UMETA(DisplayName = "Strafe Right"),
	STRAFE_RANDOM	UMETA(DisplayName = "Strafe Random")
};

UCLASS()
class ZGP_API UBTTask_Moving : public UBTTask_Base
{
	GENERATED_BODY()
	
public:
	UBTTask_Moving();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Moving")
	EMoveType m_eMoveType = EMoveType::STRAFE_RANDOM;

	// Move Away : 후퇴
	UPROPERTY(EditAnywhere, Category = "Move Away", meta = (ClampMin = "50.0"))
	float m_fMoveAwayDistance = 200.f;

	// Strafe : 타겟 기준 이동 각
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "15.0"))
	float m_fStrafeAngle = 45.f;

	// 이동 속도 배율(1.0 = 원래 속도)
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float m_fSpeedMultiply = 0.4f;

	UPROPERTY(EditAnywhere, Category = "Moving", meta = (ClampMin = "10.0"))
	float m_fAllowRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = "Moving", meta = (ClampMin = "0.1"))
	float m_fTimeout = 3.f;

private:
	FVector CalculateDestination(APawn* Pawn, const AActor* Target) const;
	FVector CalculateMoveAwayLocation(APawn* Pawn, const AActor* Target) const;
	FVector CalculateStrafeLocation(APawn* Pawn, const AActor* Target) const;

	void ApplySpeedMultiply(APawn* Pawn);
	void RestoreSpeed(APawn* Pawn);

	float m_fElapsedTime = 0.f;
	float m_fOriginalSpeed = 0.f;


};
