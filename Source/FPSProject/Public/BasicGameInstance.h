#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BasicGameInstance.generated.h"

UCLASS()
class FPSPROJECT_API UBasicGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    int32 TotalScore = 0;
    int32 TotalSleepCount = 0; 
    int32 TotalKillCount = 0;  

    void AddScore(int Amount) { TotalScore += Amount; }
    void AddKill() { TotalKillCount++; }
    void AddSleep() { TotalSleepCount++; }
};
