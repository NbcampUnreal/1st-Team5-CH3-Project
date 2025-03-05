#include "StunGun.h"

AStunGun::AStunGun()
{
    WeaponType = EWeaponType::StunGun;
    MaxAmmo = 5;
    CurrentAmmo = MaxAmmo;
}

void AStunGun::Fire()
{
    Super::Fire();
}
