#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "FPSProjectile.generated.h"

UCLASS()
class FPSPROJECT_API AFPSProjectile : public AActor
{
    GENERATED_BODY()

public:
    // �� ���� ������Ƽ�� ����Ʈ�� ����
    AFPSProjectile();

protected:
    // ���� ���� �Ǵ� ���� �� ȣ��
    virtual void BeginPlay() override;

public:
    // �����Ӹ��� ȣ��
    virtual void Tick(float DeltaTime) override;

    // ���Ǿ� �ݸ��� ������Ʈ
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;

    // �߻�ü �̵� ������Ʈ
    UPROPERTY(VisibleAnywhere, Category = Movement)
    UProjectileMovementComponent* ProjectileMovementComponent;

    // �߻� ���������� �߻�ü �ӵ��� �ʱ�ȭ�ϴ� �Լ��Դϴ�.
    void FireInDirection(const FVector& ShootDirection);

    // �߻�ü �޽�
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    UStaticMeshComponent* ProjectileMeshComponent;

    // �߻�ü ��Ƽ����
    UPROPERTY(VisibleDefaultsOnly, Category = Movement)
    UMaterialInstanceDynamic* ProjectileMaterialInstance;

    // �߻�ü�� ��򰡿� �ε��� �� ȣ��Ǵ� �Լ��Դϴ�.
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};