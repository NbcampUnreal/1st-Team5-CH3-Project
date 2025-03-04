#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BasicGameState.generated.h"

class ACoinItem;
class AFPSPlayerController;
class UBasicGameInstance;


UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	Tutorial UMETA(DisplayName = "Tutorial"),
	Stealth UMETA(DisplayName = "Stealth"),  
	Combat UMETA(DisplayName = "Combat"),    
	Escape UMETA(DisplayName = "Escape"),   
	GameOver UMETA(DisplayName = "Game Over")     
};

UCLASS()
class FPSPROJECT_API ABasicGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABasicGameState();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	// Record
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 KillCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	int32 SleepCount;
	UPROPERTY(BlueprintReadOnly, Category = "Timer")
	float CurrentPlayTime = 0.f; 
	// GamePhase
	UPROPERTY(BlueprintReadOnly, Category = "Game Phase")
	EGamePhase CurrentPhase = EGamePhase::Tutorial;
	UPROPERTY(BlueprintReadOnly, Category = "Game Phase|Mission")
	FString CurrentMissionText;



	UFUNCTION(BlueprintPure, Category = " Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = " Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = " Level")
	void OnGameOver();
	UFUNCTION(BlueprintCallable, Category = " Phase")
	void SetGamePhase(EGamePhase NewPhase);

	void UpdateHUD();
	void UpdateHealthHUD();
	void UpdateAmmoHUD();
	void UpdateMissionHUD();
	void UpdateWeaponHUD();


	UFUNCTION(BlueprintCallable, Category = "Timer")
	FString GetFormattedPlayTime();
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FString GetAmmoCount();
	
	void StartStealthPhase();
	void StartCombatPhase();

private:
	bool bIsPause;

	AFPSPlayerController* GetFPSPlayerController() const;
	UBasicGameInstance* GetBasicGameInstance() const;
};
