#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;

UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFPSPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* PauseAction;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* Viewpoint_TransformationAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SelectWeapon1Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SelectWeapon2Action;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* ReloadAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* HoldAnyKeyAction;

	//Widget

	// HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* HUDWidgetInstance;
	
	// MainMenu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu|Main")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu|Main")
	UUserWidget* MainMenuWidgetInstance;

	// Pause Menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu|Pause")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu|Pause")
	UUserWidget* PauseMenuWidgetInstance;

	//Mission
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD|Mission")
	TSubclassOf<UUserWidget> MissionWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD|Mission")
	UUserWidget* MissionWidgetInstance;

	// Game Over
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu|GameOver")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Menu|GameOver")
	UUserWidget* GameOverWidgetInstance;

	bool IsPaused;
	bool bMissionActive;


	//HUD UI
	UFUNCTION(BlueprintPure, Category = "HUD")
	UUserWidget* GetHUDWidget() const;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowGameHUD();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void ShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "GameOver")
	void ShowGameOverScreen();
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void ShowMission();
	UFUNCTION(BlueprintCallable, Category = "Mission")
	void HideMission();

	UFUNCTION(BlueprintCallable)
	void StartGame();
	UFUNCTION(BlueprintCallable)
	void QuitGame();
	UFUNCTION(BlueprintCallable)
	void TogglePauseMenu();
	



	UFUNCTION(BlueprintCallable, Category = "Timer")
	FString GetCurrentPlayTime();



	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
