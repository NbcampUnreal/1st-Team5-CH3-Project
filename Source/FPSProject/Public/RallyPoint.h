#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasicGameState.h"
#include "RallyPoint.generated.h"

class UBoxComponent;


UCLASS()
class FPSPROJECT_API ARallyPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	ARallyPoint();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CollisionBox;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	EGamePhase NextPhase;  

public:	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

};
