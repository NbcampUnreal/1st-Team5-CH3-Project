#include "RallyPoint.h"
#include "BasicGameState.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ARallyPoint::ARallyPoint()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(Scene);
	CollisionBox->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ARallyPoint::OnOverlapBegin);


	PrimaryActorTick.bCanEverTick = false;

}

void ARallyPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARallyPoint::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(AFPSCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("플레이어가 랠리포인트를 통과함!"));

		if (ABasicGameState* BasicGameState = GetWorld()->GetGameState<ABasicGameState>())
		{
			BasicGameState->SetGamePhase(NextPhase);
			if (GEngine)
			{ 
				FString PhaseMessage = FString::Printf(TEXT("Phase Changed: %s"), *UEnum::GetValueAsString(BasicGameState->CurrentPhase));
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, PhaseMessage);
			}


		}

		Destroy();
	}
}


