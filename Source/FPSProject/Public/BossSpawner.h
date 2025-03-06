#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossCharacter.h"
#include "BasicGameState.h"
#include "BossSpawner.generated.h"

class ABasicGameState;

UCLASS()
class FPSPROJECT_API ABossSpawner : public AActor
{
    GENERATED_BODY()
    
public:    
    ABossSpawner();

protected:
    // 게임 시작 시 호출
    virtual void BeginPlay() override;

public:    
    // 매 프레임 호출
    virtual void Tick(float DeltaTime) override;

    // 보스 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|Spawning")
    ABossCharacter* SpawnBoss();

    // 타이머에서 호출할 보스 스폰 함수
    UFUNCTION()
    void SpawnBossForTimer();

protected:
    // 스폰할 보스 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Spawning")
    TSubclassOf<ABossCharacter> BossClass;
    
    // 스폰 지연 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Spawning")
    float SpawnDelay = 10.0f;
    
    // 스폰 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Spawning")
    FVector SpawnOffset = FVector(0.0f, 0.0f, 100.0f);
    
    // 스폰된 보스 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Spawning")
    ABossCharacter* SpawnedBoss;
    
    // 스폰 타이머 핸들
    FTimerHandle SpawnTimerHandle;
    
    // 보스 스폰 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Spawning")
    bool bHasSpawnedBoss;
    
    // 이전 게임 페이즈
    UPROPERTY(VisibleAnywhere, Category = "Boss|Spawning")
    EGamePhase PreviousGamePhase;
    
    // 컴뱃 페이즈 감지 여부
    UPROPERTY(VisibleAnywhere, Category = "Boss|Spawning")
    bool bHasDetectedCombatPhase;
    
    // 게임 상태 캐싱
    UPROPERTY(VisibleAnywhere, Category = "Boss|Spawning")
    ABasicGameState* CachedGameState;
}; 