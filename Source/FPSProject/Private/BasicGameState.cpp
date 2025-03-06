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
#include "BossSpawner.h"

ABasicGameState::ABasicGameState()
{
    Score = 0;
    KillCount = 0;
    SleepCount = 0;
    bIsPause = false;

    PrimaryActorTick.bCanEverTick = true;
    CurrentMissionText = TEXT("미션 : ");
}

void ABasicGameState::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::BeginPlay() 실행됨! 현재 맵: %s"), *GetWorld()->GetMapName());
    
    GetFPSPlayerController()->ShowGameHUD();
    UpdateHUD();
    StartTutorialPhase();
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

void ABasicGameState::SetGamePhase(EGamePhase NewPhase)
{
    if (CurrentPhase == NewPhase) return;

    CurrentPhase = NewPhase;

    switch (CurrentPhase)
    {
    case EGamePhase::Stealth:
        StartStealthPhase();
        break;
    case EGamePhase::Combat:
        StartCombatPhase();
        break;
    case EGamePhase::GameOver:
        StartGameOverPhase();
        break;
    case EGamePhase::Boss:
        StartBossPhase();
        break;
    case EGamePhase::GameClear:
        StartGameClearPhase();
        break;
    }
}

void ABasicGameState::StartTutorialPhase()
{
    CurrentMissionText = TEXT("임무: 가옥에 잠입하기");
    GetFPSPlayerController()->ShowMission();
    UpdateMissionHUD();
}

void ABasicGameState::StartStealthPhase()
{
    CurrentMissionText = TEXT("임무: 들키지 않고 안채 깊숙한 곳에서 중요한 문서를 찾기");
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        FPSPlayerController->ShowMission();
        if (AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(GetFPSPlayerController()->GetPawn()))
        {
            FPSCharacter->SetMaxHealth(20.f);
            FPSCharacter->SetHealth(20.f);
        }
    }
    UpdateMissionHUD();
}

void ABasicGameState::StartCombatPhase()
{
    CurrentMissionText = TEXT("임무: 가옥을 탈출하기");
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        FPSPlayerController->ShowMission();
        if (AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(GetFPSPlayerController()->GetPawn()))
        {
            FPSCharacter->SetMaxHealth(200.f);
            FPSCharacter->SetHealth(200.f);
        }
    }
    UpdateMissionHUD();
}

void ABasicGameState::StartGameOverPhase()
{
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        FPSPlayerController->ShowMission();
    }
    GetWorldTimerManager().SetTimer(
        GameOverTimerHandle,
        this,
        &ABasicGameState::OnGameOver,
        3.0f,
        false
    );
}

void ABasicGameState::StartBossPhase()
{
    ABossSpawner* BossSpawner = Cast<ABossSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ABossSpawner::StaticClass()));
    if (BossSpawner)
    {
        BossSpawner->SpawnBoss();
        CurrentMissionText = TEXT("포도 대장을 처치 하고 이곳을 빠져나가기.");
        UpdateMissionHUD();
    }
}

void ABasicGameState::StartGameClearPhase()
{
    if (AFPSPlayerController* FPSPlayerController = GetFPSPlayerController())
    {
        FPSPlayerController->ShowMission();
    }

    UBasicGameInstance* BasicGameInstace = GetBasicGameInstance();
    BasicGameInstace->bIsGameClear = true;

    GetWorldTimerManager().SetTimer(
        GameOverTimerHandle,
        this,
        &ABasicGameState::OnGameOver,
        5.0f,
        false
    );
}


AFPSPlayerController* ABasicGameState::GetFPSPlayerController() const
{
    return Cast<AFPSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

UBasicGameInstance* ABasicGameState::GetBasicGameInstance() const
{
    return Cast<UBasicGameInstance>(GetGameInstance());
}
