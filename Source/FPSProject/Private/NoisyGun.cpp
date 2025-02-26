#include "NoisyGun.h"

ANoisyGun::ANoisyGun()
{
    WeaponType = EWeaponType::AggroTool;
    MaxAmmo = 3; // 한정된 개수
    CurrentAmmo = MaxAmmo;
}

void ANoisyGun::Fire()
{
    if (CurrentAmmo > 0)
    {
        CurrentAmmo--;
        UE_LOG(LogTemp, Warning, TEXT("A "));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No Ammo Left!"));
    }
}
