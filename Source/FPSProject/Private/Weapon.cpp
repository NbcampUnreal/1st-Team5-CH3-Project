#include "Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Fire()
{
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("%s Fired! Ammo: %d/%d"), *GetName(), CurrentAmmo, MaxAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of Ammo! Reload Needed."));
	}
}

void AWeapon::Reload()
{
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogTemp, Warning, TEXT("%s Reloaded! Ammo: %d/%d"), *GetName(), CurrentAmmo, MaxAmmo);
}

void AWeapon::Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Equipped"), *GetName());
}
