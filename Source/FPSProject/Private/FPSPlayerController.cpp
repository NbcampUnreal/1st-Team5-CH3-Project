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
    HoldAnyKeyAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MainMenuWidgetClass(nullptr),
    MainMenuWidgetInstance(nullptr),
    GameOverWidgetClass(nullptr),
    GameOverWidgetInstance(nullptr),
    IsPaused(false),
    bMissionActive(false)
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
    EnhancedInput->BindAction(HoldAnyKeyAction, ETriggerEvent::Triggered, this, &AFPSPlayerController::HideMission);
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
        ABasicGameState* BasicGameState = GetWorld() ? GetWorld()->GetGameState<ABasicGameState>() : nullptr;
        if (BasicGameState) {
            BasicGameState->UpdateHUD();
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

        if (UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this)))
        {
            if (BasicGameInstance->bIsGameClear)
            {
                if (UTextBlock* GameOverText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("GameOverText"))))
                {
                    GameOverText->SetText(FText::FromString(TEXT("임무 성공")));

                    FSlateColor BlueColor = FSlateColor(FLinearColor(0.0f, 0.0f, 1.0f, 1.0f));
                    GameOverText->SetColorAndOpacity(BlueColor);
                }
            }
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
                        FString::Printf(TEXT("잠재운 적 수 : %d"), BasicGameInstance->TotalSleepCount)
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

void AFPSPlayerController::ShowMission()
{
    UE_LOG(LogTemp,Warning, TEXT("ShowMission()"))
    MissionWidgetInstance = CreateWidget<UUserWidget>(this, MissionWidgetClass);
    if (MissionWidgetInstance)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(MissionWidgetInstance->TakeWidget()); // UI 포커스 유지
        SetInputMode(InputMode);

        MissionWidgetInstance->AddToViewport();
        //Text Settings
        FString DisplayText1;
        FString DisplayText2;
        FString DisplayText3;
        FString DisplayText4;
        FString DisplayText5;
        ABasicGameState* BasicGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(this));
        if (BasicGameState)
        {
            switch (BasicGameState->CurrentPhase)
            {
                case EGamePhase::Tutorial:
                    DisplayText1 = TEXT("밤이 깊었다. 관아의 경비가 조금씩 느슨해지고 있어.");
                    DisplayText2 = TEXT("이 집에는 백성들의 피와 땀을 착취하는 문서들이 잔뜩 있다지.");
                    DisplayText3 = TEXT("어서 잡입해보자.");
                    DisplayText4 = TEXT("");
                    DisplayText5 = TEXT("");
                    break;
                case EGamePhase::Stealth:
                    DisplayText1 = TEXT("무언가 수상해.왜 이렇게 많은 포졸들이 지키고 있는거지?");
                    DisplayText2 = TEXT("역시 무언가 수상해.");
                    DisplayText3 = TEXT("경비병 에게 다가가면 들키고 말꺼야");
                    DisplayText4 = TEXT("바로 왼쪽 뒷길을 이용하여 잡입해보자");
                    DisplayText5 = TEXT("");
                    break;
                case EGamePhase::Combat:
                    DisplayText1 = TEXT("경비병 : 침입자가 나타났다!!");
                    DisplayText2 = TEXT("경비병 : 침입자를 처단하라!!");
                    DisplayText3 = TEXT("");
                    DisplayText4 = TEXT("");
                    DisplayText5 = TEXT("");
                    break;
                case EGamePhase::GameOver:
                    DisplayText1 = TEXT("경비병 : 네 이놈 여기가 어디라고 들어오느냐!!");
                    DisplayText2 = TEXT("경비병 : 이제 사람 구실을 못하게 만들어주마.");
                    DisplayText3 = TEXT("");
                    DisplayText4 = TEXT("");
                    DisplayText5 = TEXT("");
                    break;
                case EGamePhase::GameClear:
                    DisplayText1 = TEXT("포도대장을 쓰러뜨리고 탈출에 성공했다.");
                    DisplayText2 = TEXT("동료들이 나타나 길을 열었다.");
                    DisplayText3 = TEXT("불길이 치솟는 관아를 뒤로한 채, 모두 달아났다.");
                    DisplayText4 = TEXT("자유를 쟁취한 자들, 그들의 싸움은 이제 시작이다.");
                    DisplayText5 = TEXT("");
                    UTextBlock* InformationText = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("InformationText")));
                    InformationText->SetVisibility(ESlateVisibility::Hidden);
                    break;
            }
        }

        UTextBlock* MissionTextBlock1 = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("MissionText1")));
        UTextBlock* MissionTextBlock2 = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("MissionText2")));
        UTextBlock* MissionTextBlock3 = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("MissionText3")));
        UTextBlock* MissionTextBlock4 = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("MissionText4")));
        UTextBlock* MissionTextBlock5 = Cast<UTextBlock>(MissionWidgetInstance->GetWidgetFromName(TEXT("MissionText5")));
        MissionTextBlock1->SetText(FText::FromString(DisplayText1));
        MissionTextBlock2->SetText(FText::FromString(DisplayText2));
        MissionTextBlock3->SetText(FText::FromString(DisplayText3));
        MissionTextBlock4->SetText(FText::FromString(DisplayText4));
        MissionTextBlock5->SetText(FText::FromString(DisplayText5));
        UFunction* PlayAnimFunc = MissionWidgetInstance->FindFunction(FName("PlayTextAnim"));
        if (PlayAnimFunc)
        {
            MissionWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
        }
    }
    bMissionActive = true;

}

void AFPSPlayerController::HideMission()
{
    UE_LOG(LogTemp, Warning, TEXT("HideMission()"))
    if (bMissionActive && MissionWidgetInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("(bMissionActive && MissionWidgetInstance"))
        MissionWidgetInstance->RemoveFromParent();
        MissionWidgetInstance = nullptr;

        SetPause(false);
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;

        bMissionActive = false;
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

void AFPSPlayerController::ResetGameData()
{
    UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (BasicGameInstance)
    {
        BasicGameInstance->TotalPlayTime = 0;
        BasicGameInstance->TotalKillCount = 0;
        BasicGameInstance->TotalSleepCount = 0;
        BasicGameInstance->TotalScore = 0;
        BasicGameInstance->bIsDocumentDestroyed = false;
        BasicGameInstance->bIsGameClear = false;
    }
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