#include "RallyPoint.h"
#include "BasicGameState.h"
#include "BasicGameInstance.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h"
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



		if (ABasicGameState* BasicGameState = GetWorld()->GetGameState<ABasicGameState>())
		{
			if (UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				//게임 진행이 안됐을 경우
				if (!BasicGameInstance->bIsDocumentDestroyed)
				{
					//gameOver 영역에 들어갔을 때
					if (NextPhase == EGamePhase::GameOver)
					{
						// 걸렸을 때 UI 띄우기 + 게임 오버
						BasicGameState->OnGameOver();

						return;
					}
					else if(NextPhase == EGamePhase::Combat)
					{
						//그냥 통과
						return;
					}
				}
				else
				{
					if (NextPhase == EGamePhase::GameOver)
					{
						//아무일도 x
						return;
					}


				}
				BasicGameState->SetGamePhase(NextPhase);
			}
		}

		Destroy();
	}
}


