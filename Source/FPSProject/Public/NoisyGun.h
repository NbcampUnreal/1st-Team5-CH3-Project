#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "NoisyGun.generated.h"

UCLASS()
class FPSPROJECT_API ANoisyGun : public AWeapon
{
    GENERATED_BODY()

public:
    ANoisyGun();
    virtual void Fire() override;
};
