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

    // Tick �Լ� �������̵� �߰�
    virtual void Tick(float DeltaTime) override;

    void Fire();

protected:
    // Skeletal Mesh �߰� (�ѱ� ��)
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USkeletalMeshComponent* WeaponMesh;

    // �߻� ����
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FireRange = 10000.0f;

    // �ѱ� ����Ʈ �� ����
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* MuzzleFlash;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    USoundBase* FireSound;
};
