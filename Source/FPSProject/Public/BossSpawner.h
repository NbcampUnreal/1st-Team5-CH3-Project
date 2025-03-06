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

    // 보스 스폰 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|Spawning")
    ABossCharacter* SpawnBoss();


protected:
    // 스폰할 보스 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Spawning")
    TSubclassOf<ABossCharacter> BossClass;
    
    
    // 스폰 위치 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Spawning")
    FVector SpawnOffset = FVector(0.0f, 0.0f, 100.0f);
    
    // 스폰된 보스 참조
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Spawning")
    ABossCharacter* SpawnedBoss;
    
    // 보스 스폰 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Spawning")
    bool bHasSpawnedBoss;
    
}; 