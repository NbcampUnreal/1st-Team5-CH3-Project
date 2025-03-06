#include "BossSpawner.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BossAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ABossSpawner::ABossSpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // 기본값 설정
    SpawnOffset = FVector(0.0f, 0.0f, 100.0f);
    bHasSpawnedBoss = false; 
    SpawnedBoss = nullptr;
}

void ABossSpawner::BeginPlay()
{
    Super::BeginPlay();
}


ABossCharacter* ABossSpawner::SpawnBoss()
{
    if (bHasSpawnedBoss)
    {
        return SpawnedBoss;
    }
    
    if (!BossClass)
    {
        return nullptr;
    }
    
    // 스폰 위치 계산
    FVector SpawnLocation = GetActorLocation() + SpawnOffset;
    FRotator SpawnRotation = GetActorRotation();
    
    // 스폰 파라미터 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    // 보스 스폰
    SpawnedBoss = GetWorld()->SpawnActor<ABossCharacter>(
        BossClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams);
        
    if (SpawnedBoss)
    {
        // 보스 스폰 성공 표시
        bHasSpawnedBoss = true;
        
        
        // 보스가 스폰되면 즉시 활성화 (Sleep 상태 해제)
        if (SpawnedBoss->IsSleeping())
        {
            // Sleep 상태에서 깨우기
            SpawnedBoss->WakeUp();
            
            // 보스 AI 컨트롤러 활성화
            ABossAIController* BossAI = Cast<ABossAIController>(SpawnedBoss->GetController());
            if (BossAI)
            {
                BossAI->GetBrainComponent()->RestartLogic();
            }
        }
    }
    
    return SpawnedBoss;
}
