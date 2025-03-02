#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Controller.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAIController.generated.h"

UCLASS()
class FPSPROJECT_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();
    
    // Tick 함수 추가
    virtual void Tick(float DeltaTime) override;

    // 플레이어를 볼 수 있는지 확인하는 함수
    UFUNCTION(BlueprintCallable, Category = "AI")
    bool CanSeePlayer();

    // 공격 범위 내에 있는지 확인하는 함수
    UFUNCTION(BlueprintCallable, Category = "AI")
    bool IsInAttackRange();

protected:
    virtual void BeginPlay() override;
    
    // AI가 Pawn을 제어하기 시작할 때 호출
    virtual void OnPossess(APawn* InPawn) override;
    
    // AI가 Pawn 제어를 중지할 때 호출
    virtual void OnUnPossess() override;

    // 플레이어 감지 상태 업데이트
    void UpdatePlayerDetection();

    // Behavior Tree 컴포넌트
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
    UBehaviorTreeComponent* BehaviorTreeComponent;

    // Blackboard 컴포넌트
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
    UBlackboardComponent* BlackboardComponent;

    // Behavior Tree 에셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
    UBehaviorTree* BehaviorTree;
}; 
