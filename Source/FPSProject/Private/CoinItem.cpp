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
}

void ACoinItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (UBasicGameInstance* GameInstance = World->GetGameState<UBasicGameInstance>())
            {
                GameInstance->AddScore(PointValue);
            }
        }
        DestroyItem();
    }
}
