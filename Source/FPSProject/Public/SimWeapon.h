#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimWeapon.generated.h"

UCLASS()
class FPSPROJECT_API ASimWeapon : public AActor
{
    GENERATED_BODY()

public:
    ASimWeapon();

    // Tick 함수 오버라이드 추가
    virtual void Tick(float DeltaTime) override;

    void Fire();

protected:
    // Skeletal Mesh 추가 (총기 모델)
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USkeletalMeshComponent* WeaponMesh;

    // 발사 범위
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FireRange = 10000.0f;

    // 총구 이펙트 및 사운드
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* MuzzleFlash;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    USoundBase* FireSound;
};
