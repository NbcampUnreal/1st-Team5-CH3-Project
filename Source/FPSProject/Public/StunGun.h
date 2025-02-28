#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "StunGun.generated.h"

UCLASS()
class FPSPROJECT_API AStunGun : public AWeapon
{
    GENERATED_BODY()

public:
    AStunGun();
    virtual void Fire() override;
};