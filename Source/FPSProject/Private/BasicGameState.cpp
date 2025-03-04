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
    bIsPause = false;

    PrimaryActorTick.bCanEverTick = true;
}

void ABasicGameState::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::BeginPlay() 실행됨! 현재 맵: %s"), *GetWorld()->GetMapName());
}

void ABasicGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsPause)
    {
        CurrentPlayTime += DeltaTime;
    }
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
    bIsPause = true;
    UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (BasicGameInstance)
    {
        BasicGameInstance->TotalPlayTime += CurrentPlayTime;
    }

    if (AFPSPlayerController* PlayerController = GetFPSPlayerController())
    {
        PlayerController->SetPause(true);
        
        PlayerController->ShowGameOverScreen();
    }
}

void ABasicGameState::SetGamePhase(EGamePhase NewPhase)
{
    if (CurrentPhase == NewPhase) return; 

    CurrentPhase = NewPhase;
    UE_LOG(LogTemp, Warning, TEXT("Game Phase changed to: %d"), (uint8)CurrentPhase);

    switch (CurrentPhase)
    {
    case EGamePhase::Stealth:
        break;

    case EGamePhase::Combat:
        break;

    case EGamePhase::Escape:
        break;

    case EGamePhase::GameOver:
        break;
    }
}

void ABasicGameState::UpdateHUD()
{
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        if (UUserWidget* HUDWidget = FPSPlayerController->GetHUDWidget())
        {

        }
    }
}

FString ABasicGameState::GetFormattedPlayTime()
{
    int32 TotalSeconds = FMath::RoundToInt(CurrentPlayTime);
    int32 Hours = TotalSeconds / 3600;
    int32 Minutes = (TotalSeconds % 3600) / 60;
    int32 Seconds = TotalSeconds % 60;

    return FString::Printf(TEXT("플레이 시간 : %02d:%02d:%02d"), Hours, Minutes, Seconds);
}

void ABasicGameState::StartStealthPhase()
{
    CurrentPhase = EGamePhase::Stealth;


}

void ABasicGameState::StartCombatPhase()
{
    CurrentPhase = EGamePhase::Combat;
}


AFPSPlayerController* ABasicGameState::GetFPSPlayerController() const
{
    return Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

UBasicGameInstance* ABasicGameState::GetBasicGameInstance() const
{
    return Cast<UBasicGameInstance>(GetGameInstance());
}
