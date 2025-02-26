#include "BasicGameState.h"
#include "BasicGameInstance.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ABasicGameState::ABasicGameState()
{
    Score = 0;
    KillCount = 0;
    SleepCount = 0;
}

void ABasicGameState::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::BeginPlay() 실행됨! 현재 맵: %s"), *GetWorld()->GetMapName());
}

int32 ABasicGameState::GetScore() const
{
    return Score;
}

void ABasicGameState::AddScore(int32 Amount)
{
    if (UBasicGameInstance* GI = GetBasicGameInstance())
    {
        GI->AddScore(Amount);
    }
}

void ABasicGameState::OnGameOver()
{
    if (AFPSPlayerController* PlayerController = GetFPSPlayerController())
    {
        PlayerController->SetPause(true);
        
        //PlayerController->ShowMainMenu(true);
    }
}

void ABasicGameState::UpdateHUD()
{
}

AFPSPlayerController* ABasicGameState::GetFPSPlayerController() const
{
    return Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

UBasicGameInstance* ABasicGameState::GetBasicGameInstance() const
{
    return Cast<UBasicGameInstance>(GetGameInstance());
}
