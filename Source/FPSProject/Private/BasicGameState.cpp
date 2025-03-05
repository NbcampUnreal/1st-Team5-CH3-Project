#include "BasicGameState.h"
#include "BasicGameInstance.h"
#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "CoinItem.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

ABasicGameState::ABasicGameState()
{
    Score = 0;
    KillCount = 0;
    SleepCount = 0;
    bIsPause = false;

    PrimaryActorTick.bCanEverTick = true;
    CurrentMissionText = TEXT("미션 : 잠은 죽어서 자자");
}

void ABasicGameState::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::BeginPlay() 실행됨! 현재 맵: %s"), *GetWorld()->GetMapName());
    
    GetFPSPlayerController()->ShowGameHUD();
    UpdateHUD();
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
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());

            //HP Bar
            if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
            {
                float CurrentHP = FPSCharacter->GetHealth();
                float MaxHP = FPSCharacter->GetMaxHealth();
                float HPPercent = CurrentHP / MaxHP;
                HPBar->SetPercent(HPPercent);
            }

            //Crosshair
            if (UImage* CrosshairImage = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("CrosshairImage"))))
            {
                FString AssetPath = "/Game/Image/sniper-297661.sniper-297661";
                UTexture2D* LoadedTexture = LoadObject<UTexture2D>(nullptr, *AssetPath);
                if (LoadedTexture)
                {
                    CrosshairImage->SetBrushFromTexture(LoadedTexture);
                }

            }

            //Mission Text
            if (UTextBlock* MissionText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("MissionText"))))
            {
                MissionText->SetText(FText::FromString(CurrentMissionText));
            }
        }
    }
}

void ABasicGameState::UpdateHealthHUD()
{
    
    if (AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        if (UUserWidget* HUDWidget = FPSPlayerController->GetHUDWidget())
        {
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());
            if (FPSCharacter)
            {
                if (UProgressBar* HPBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("HPBar"))))
                {
                    float CurrentHP = FPSCharacter->GetHealth();
                    float MaxHP = FPSCharacter->GetMaxHealth();
                    float HPPercent = CurrentHP / MaxHP;
                    HPBar->SetPercent(HPPercent);
                }
            }
        }
    }
}

void ABasicGameState::UpdateAmmoHUD()
{
    if (AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        if (UUserWidget* HUDWidget = FPSPlayerController->GetHUDWidget())
        {
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());
            if (FPSCharacter)
            {
                // CurrentAmmo Text
                if (UTextBlock* CurrentAmmoText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("CurrentAmmoText"))))
                {
                    int32 CurrentAmmo = FPSCharacter->CurrentWeapon->GetCurrentAmmo();
                    CurrentAmmoText->SetText(FText::FromString(
                        FString::Printf(TEXT("%d"), CurrentAmmo)
                    ));
                }
                // TotalAmmo Text
                if (UTextBlock* TotalAmmoText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TotalAmmoText"))))
                {
                    int32 TotalAmmo = FPSCharacter->CurrentWeapon->GetTotalAmmo();
                    TotalAmmoText->SetText(FText::FromString(
                        FString::Printf(TEXT("%d"), TotalAmmo)
                    ));
                }
            }
        }
    }
}

void ABasicGameState::UpdateMissionHUD()
{
    if (AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        if (UUserWidget* HUDWidget = FPSPlayerController->GetHUDWidget())
        {
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());
            if (FPSCharacter)
            {
                //Mission Text
                if (UTextBlock* MissionText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("MissionText"))))
                {
                    MissionText->SetText(FText::FromString(CurrentMissionText));
                }
            }
        }
    }
}

void ABasicGameState::UpdateWeaponHUD()
{
    if (AFPSPlayerController* FPSPlayerController = Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
    {
        if (UUserWidget* HUDWidget = FPSPlayerController->GetHUDWidget())
        {
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());
            if (FPSCharacter)
            {
                // Weapon Image, WeaponText
                if (UImage* WeaponImage = Cast<UImage>(HUDWidget->GetWidgetFromName(TEXT("WeaponImage"))))
                {
                    FString AssetPath;

                    if (UTextBlock* WeaponText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WeaponText"))))
                    {
                        switch (FPSCharacter->CurrentWeapon->GetWeaponType())
                        {
                        case EWeaponType::NormalGun:
                            AssetPath = "/Game/Image/GunImage.GunImage";
                            WeaponText->SetText(FText::FromString(
                                FString::Printf(TEXT("소총"))
                            ));
                            break;
                        case EWeaponType::StunGun:
                            AssetPath = "/Game/Image/StunGun.StunGun";
                            WeaponText->SetText(FText::FromString(
                                FString::Printf(TEXT("마취총"))
                            ));
                            break;
                        }
                        UTexture2D* LoadedTexture = LoadObject<UTexture2D>(nullptr, *AssetPath);
                        if (LoadedTexture)
                        {
                            WeaponImage->SetBrushFromTexture(LoadedTexture);
                        }
                    }
                }
            }
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

FString ABasicGameState::GetAmmoCount()
{
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(FPSPlayerController->GetPawn());
        if (FPSCharacter)
        {
            //return FString::Printf(TEXT("%d / %d"), FPSCharacter->CurrentAmmo, FPSCharacter->MaxAmmo);
        }
    }
    return FString(TEXT("0 / 0"));
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
