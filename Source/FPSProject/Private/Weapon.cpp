#include "Weapon.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	RemainingTotalAmmo = 20;
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
		UE_LOG(LogTemp, Warning, TEXT("Fire Ammo"));
		CurrentAmmo--;
	}
	else
	{
		//Reload 알림이나 소리
	}
}

void AWeapon::Reload()
{
	int32 ReloadCount = std::min(MaxAmmo-CurrentAmmo, RemainingTotalAmmo);
	RemainingTotalAmmo -= ReloadCount;
	CurrentAmmo += ReloadCount;
}

void AWeapon::Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Equipped"), *GetName());
}
