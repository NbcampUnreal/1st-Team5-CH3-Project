#include "NormalGun.h"

ANormalGun::ANormalGun()
{
    WeaponType = EWeaponType::NormalGun;
    MaxAmmo = 12;
    CurrentAmmo = MaxAmmo;
}

void ANormalGun::Fire()
{
    Super::Fire();
}
