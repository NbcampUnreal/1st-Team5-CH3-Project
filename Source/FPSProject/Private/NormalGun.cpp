#include "NormalGun.h"

ANormalGun::ANormalGun()
{
    WeaponType = EWeaponType::NormalGun;
    MaxAmmo = 12;
    CurrentAmmo = MaxAmmo;
}

void ANormalGun::Fire()
{
    if (CurrentAmmo > 0)
    {
        CurrentAmmo--;
        UE_LOG(LogTemp, Warning, TEXT("Gun Fired! Loud Noise Alert! Ammo: %d/%d"), CurrentAmmo, MaxAmmo);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Gun Out of Ammo! Reload Needed."));
    }
}
