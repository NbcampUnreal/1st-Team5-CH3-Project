#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Attack.generated.h"

UCLASS()
class FPSPROJECT_API UBTTask_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 버스트 발사 간격
	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	float BurstCooldown = 0.5f;

	// 최소/최대 버스트 발사 횟수
	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	int32 MinBurstCount = 1;

	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	int32 MaxBurstCount = 4;

	// 패턴 대기 시간
	UPROPERTY(EditAnywhere, Category = "AI|Attack")
	float PatternCooldown = 2.0f;

	// 공격 애니메이션 몽타주 (나중에 리타겟팅된 총쏘기 애니메이션용)
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ShootMontage;

private:
	float LastAttackTime = 0.0f;
	int32 CurrentBurstTarget = 0;  // 현재 버스트의 목표 발사 횟수
};
