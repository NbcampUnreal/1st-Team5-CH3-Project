#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Controller.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "EnemyAIController.generated.h"

UCLASS()
class FPSPROJECT_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

protected:
    virtual void BeginPlay() override;
    
    // AI가 Pawn을 제어하기 시작할 때 호출
    virtual void OnPossess(APawn* InPawn) override;
    
    // AI가 Pawn 제어를 중지할 때 호출
    virtual void OnUnPossess() override;

    // AI Perception Component
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    UAIPerceptionComponent* AIPerceptionComponent;

    // 시야 감지 설정
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Perception")
    UAISenseConfig_Sight* SightConfig;

    // 감지 이벤트 처리 함수
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
}; 
