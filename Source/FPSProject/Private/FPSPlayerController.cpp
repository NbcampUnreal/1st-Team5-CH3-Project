#include "FPSPlayerController.h"
#include "BasicGameInstance.h"
#include "BasicGameState.h"
#include "EnhancedInputSubsystems.h"   
#include "EnhancedInputComponent.h"    
#include "FPSCharacter.h"   
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

AFPSPlayerController::AFPSPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    CrouchAction(nullptr),
    Viewpoint_TransformationAction(nullptr),
    SelectWeapon1Action(nullptr),
    SelectWeapon2Action(nullptr),
    FireAction(nullptr),
    ReloadAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MainMenuWidgetClass(nullptr),
    MainMenuWidgetInstance(nullptr),
    GameOverWidgetClass(nullptr),
    GameOverWidgetInstance(nullptr)
{
}

void AFPSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext"));
            }
        }
    }

    FString CurrentMapName = GetWorld()->GetMapName();
    if (CurrentMapName.Contains("MenuLevel"))
    {
        ShowMainMenu();
    }
}

void AFPSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent(); // 기본 입력 설정 유지

    // ESC 키를 누르면 일시 정지
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started, this, &AFPSPlayerController::TogglePauseMenu);
}

UUserWidget* AFPSPlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void AFPSPlayerController::ShowGameHUD()
{
    UE_LOG(LogTemp, Warning, TEXT("ShowGameHUD()!!"));
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    if (MainMenuWidgetInstance)
    {
        MainMenuWidgetInstance->RemoveFromParent();
        MainMenuWidgetInstance = nullptr;
    }

    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();

            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
        }
        UE_LOG(LogTemp, Warning, TEXT("Create WidgetInstance"));
        ABasicGameState* BasicGameState = GetWorld() ? GetWorld()->GetGameState<ABasicGameState>() : nullptr;
        if (BasicGameState) {
            BasicGameState->UpdateHUD();
            UE_LOG(LogTemp, Warning, TEXT("UpdateHUD "));
        }
    }
}

void AFPSPlayerController::ShowMainMenu()
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    if (MainMenuWidgetClass)
    {
        MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidgetInstance)
        {
            MainMenuWidgetInstance->AddToViewport();

            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }
    }
}

void AFPSPlayerController::ShowGameOverScreen()
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }

    if (GameOverWidgetInstance)
    {
        GameOverWidgetInstance->RemoveFromParent();
        GameOverWidgetInstance = nullptr;
    }

    if (GameOverWidgetClass)
    {
        GameOverWidgetInstance = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
        if (GameOverWidgetInstance)
        {
            GameOverWidgetInstance->AddToViewport();

            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }

        UFunction* PlayAnimFunc = GameOverWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
        if (PlayAnimFunc)
        {
            GameOverWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
            UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this));

            if (UTextBlock* PlayTimeText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("PlayTimeText"))))
            {
                PlayTimeText->SetText(FText::FromString(GetCurrentPlayTime()));
            }
            if (UTextBlock* KillCountText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("KillCountText"))))
            {
                //KillCount
                if (BasicGameInstance)
                {
                    KillCountText->SetText(FText::FromString(
                        FString::Printf(TEXT("처치한 적 수 : %d"), BasicGameInstance->TotalKillCount)
                    ));
                }
            }
            if (UTextBlock* SleepCountText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("SleepCountText"))))
            {
                if (BasicGameInstance)
                {
                    SleepCountText->SetText(FText::FromString(
                        FString::Printf(TEXT("잠재운 적 수 : %d"), BasicGameInstance->TotalKillCount)
                    ));
                }
            }
            if (UTextBlock* ScoreText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("ScoreText"))))
            {
                //Score
                if (BasicGameInstance)
                {
                    ScoreText->SetText(FText::FromString(
                        FString::Printf(TEXT("점수 : %d"), BasicGameInstance->TotalScore)
                    ));
                }
            }
        }
    }
}

void AFPSPlayerController::StartGame()
{
    if (UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        BasicGameInstance->TotalPlayTime = 0;
        BasicGameInstance->TotalKillCount = 0;
        BasicGameInstance->TotalSleepCount = 0;
        BasicGameInstance->TotalScore = 0;
    }

    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Demo"));
    SetPause(false);
    SetInputMode(FInputModeGameOnly());

}



void AFPSPlayerController::QuitGame()
{
    UKismetSystemLibrary::QuitGame(
        GetWorld(),
        this,
        EQuitPreference::Quit,
        false
    );
}

void AFPSPlayerController::TogglePauseMenu()
{
    if (IsPaused)
    {
        // 게임 재개
        SetPause(false);
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;

        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->RemoveFromParent();
            PauseMenuWidgetInstance = nullptr;
        }
    }
    else
    {
        // 게임 일시정지 & UI 표시
        SetPause(true);
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;

        if (!PauseMenuWidgetClass) return;

        PauseMenuWidgetInstance = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
        if (PauseMenuWidgetInstance)
        {
            PauseMenuWidgetInstance->AddToViewport();
        }
    }

    IsPaused = !IsPaused;
}


FString AFPSPlayerController::GetCurrentPlayTime()
{
    ABasicGameState* BasicGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(this));
    if (BasicGameState)
    {
        return BasicGameState->GetFormattedPlayTime(); 
    }
    return FString(TEXT("00:00:00"));
}