#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "NormalGun.generated.h"

UCLASS()
class FPSPROJECT_API ANormalGun : public AWeapon
{
    GENERATED_BODY()

public:
    ANormalGun();
    virtual void Fire() override;

    virtual void Reload() override;  // 탄약 재장전
};
