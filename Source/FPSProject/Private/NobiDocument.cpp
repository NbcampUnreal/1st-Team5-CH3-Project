#include "NobiDocument.h"
#include "Engine/World.h"
#include "BasicGameState.h"
#include "BasicGameInstance.h"

ANobiDocument::ANobiDocument()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemType = "NobiDocument";
    RotationSpeed = 50.f;
}

void ANobiDocument::BeginPlay()
{
	Super::BeginPlay();
	
}

void ANobiDocument::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (UWorld* World = GetWorld())
        {
            if (UBasicGameInstance* GameInstance = World->GetGameInstance<UBasicGameInstance>())
            {
                GameInstance->bIsDocumentDestroyed = true;
            }
        }
        DestroyItem();
    }
}

void ANobiDocument::Tick(float DeltaTime)
{
    AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaTime, 0.0f));
}


