#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Controller.h"
#include "Navigation/PathFollowingComponent.h"
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
}; 
