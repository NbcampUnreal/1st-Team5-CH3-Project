#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Default values
    WeaponName = "Default Weapon";
    AmmoCount = 30;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeapon::Use()
{
    if (AmmoCount > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s Used! Ammo Left: %d"), *WeaponName, AmmoCount - 1);
        AmmoCount--;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s is out of ammo!"), *WeaponName);
    }
}
