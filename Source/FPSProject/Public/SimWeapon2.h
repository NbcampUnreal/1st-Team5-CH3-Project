#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimWeapon2.generated.h"

UCLASS()
class FPSPROJECT_API ASimWeapon2 : public AActor
{
    GENERATED_BODY()

public:
    ASimWeapon2();

protected:
    virtual void Tick(float DeltaTime) override;

public:
    void Fire();

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FireRange = 10000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float DamageAmount = 20.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* MuzzleFlash;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    USoundBase* FireSound;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    class USkeletalMeshComponent* WeaponMesh;
};
