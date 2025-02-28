#include "StunGun.h"

AStunGun::AStunGun()
{
    WeaponType = EWeaponType::StunGun;
    MaxAmmo = 5;
    CurrentAmmo = MaxAmmo;
}

void AStunGun::Fire()
{
    if (CurrentAmmo > 0)
    {
        CurrentAmmo--;
        UE_LOG(LogTemp, Warning, TEXT("Stun Gun Fired! Target Stunned."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Stun Gun Out of Ammo!"));
    }
}
