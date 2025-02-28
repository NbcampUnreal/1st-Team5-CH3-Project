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

            if (UTextBlock* PlayTimeText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("PlayTimeText"))))
            {
                //PlayTime
            }
            if (UTextBlock* KillCountText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("KillCountText"))))
            {
                //KillCount
            }
            if (UTextBlock* SleepCountText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("SleepCountText"))))
            {
                //SleepCount
            }
            if (UTextBlock* ScoreText = Cast<UTextBlock>(GameOverWidgetInstance->GetWidgetFromName(TEXT("ScoreText"))))
            {//a
                //Score
                if (UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this)))
                {
                    ScoreText->SetText(FText::FromString(
                        FString::Printf(TEXT("Score : %d"), BasicGameInstance->TotalScore)
                    ));
                }
            }
        }
    }
}