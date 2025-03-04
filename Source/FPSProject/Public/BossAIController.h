#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "BossAIController.generated.h"

UCLASS()
class FPSPROJECT_API ABossAIController : public AEnemyAIController
{
    GENERATED_BODY()

public:
    ABossAIController();

    // Tick 함수 오버라이드
    virtual void Tick(float DeltaTime) override;

    // 보스 전용 플레이어 감지 함수 (더 넓은 범위)
    UFUNCTION(BlueprintCallable, Category = "AI|Boss")
    bool CanSeePlayerBoss();

    // 공격 범위 체크 함수
    bool IsInAttackRange();

protected:
    virtual void BeginPlay() override;

    // AI가 Pawn을 제어하기 시작할 때 호출
    virtual void OnPossess(APawn *InPawn) override;

    // 보스 전용 플레이어 감지 상태 업데이트
    void UpdateBossPlayerDetection();

    // 보스 전용 행동 트리 에셋
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
    class UBehaviorTree *BossBehaviorTree;
};