#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BasicGameState.generated.h"

class ACoinItem;
class AFPSPlayerController;
class UBasicGameInstance;

UCLASS()
class FPSPROJECT_API ABasicGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABasicGameState();
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 KillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 SleepCount;

	UFUNCTION(BlueprintPure, Category = " Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = " Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = " Level")
	void OnGameOver();


	void UpdateHUD();


private:
	AFPSPlayerController* GetFPSPlayerController() const;
	UBasicGameInstance* GetBasicGameInstance() const;
};
