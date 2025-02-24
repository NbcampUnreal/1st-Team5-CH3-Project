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
    // 이 액터 프로퍼티의 디폴트값 설정
    AFPSProjectile();

protected:
    // 게임 시작 또는 스폰 시 호출
    virtual void BeginPlay() override;

public:
    // 프레임마다 호출
    virtual void Tick(float DeltaTime) override;

    // 스피어 콜리전 컴포넌트
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    USphereComponent* CollisionComponent;

    // 발사체 이동 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = Movement)
    UProjectileMovementComponent* ProjectileMovementComponent;

    // 발사 방향으로의 발사체 속도를 초기화하는 함수입니다.
    void FireInDirection(const FVector& ShootDirection);

    // 발사체 메시
    UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
    UStaticMeshComponent* ProjectileMeshComponent;

    // 발사체 머티리얼
    UPROPERTY(VisibleDefaultsOnly, Category = Movement)
    UMaterialInstanceDynamic* ProjectileMaterialInstance;

    // 발사체가 어딘가에 부딪힐 때 호출되는 함수입니다.
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};