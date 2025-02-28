#include "CoinItem.h"
#include "Engine/World.h"
#include "BasicGameState.h"
#include "BasicGameInstance.h"

ACoinItem::ACoinItem()
    : PointValue(1000)
{
    ItemType = "Coin";

}

void ACoinItem::BeginPlay()
{
    Super::BeginPlay();
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (UBasicGameInstance* GameInstance = World->GetGameInstance<UBasicGameInstance>())
            {
                GameInstance->AddScore(PointValue);
                UE_LOG(LogTemp, Warning, TEXT("%d"), GameInstance->TotalScore);
            }
        }
        DestroyItem();
    }
}
