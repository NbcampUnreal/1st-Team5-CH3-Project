#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS(Blueprintable)
class FPSPROJECT_API AWeapon : public AActor  
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWeapon();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual void Use();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
    FString WeaponName;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
    int32 AmmoCount;
};
