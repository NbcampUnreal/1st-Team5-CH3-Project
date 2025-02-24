#include "FPSProjectile.h"

// 디폴트값 설정
AFPSProjectile::AFPSProjectile()
{
    // 이 액터가 프레임마다 Tick()을 호출하도록 설정합니다.  이 설정이 필요 없는 경우 비활성화하면 퍼포먼스가 향상됩니다.
    PrimaryActorTick.bCanEverTick = false;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        // 스피어를 단순 콜리전 표현으로 사용합니다.
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // 스피어의 콜리전 프로파일 이름을 'Projectile'로 설정합니다.
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        // 컴포넌트가 어딘가에 부딪힐 때 호출되는 이벤트입니다.
        CollisionComponent->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);
        // 스피어의 콜리전 반경을 설정합니다.
        CollisionComponent->InitSphereRadius(15.0f);
        // 루트 컴포넌트가 콜리전 컴포넌트가 되도록 설정합니다.
        RootComponent = CollisionComponent;
    }

    if (!ProjectileMovementComponent)
    {
        // 이 컴포넌트를 사용하여 이 발사체의 이동을 주도합니다.
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
        ProjectileMovementComponent->InitialSpeed = 3000.0f;
        ProjectileMovementComponent->MaxSpeed = 3000.0f;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.3f;
        ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    }

    if (!ProjectileMeshComponent)
    {
        ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
        static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Sphere.Sphere'"));
        if (Mesh.Succeeded())
        {
            ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
        }

        static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/SphereMaterial.SphereMaterial'"));
        if (Material.Succeeded())
        {
            ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
        }
        ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
        ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
        ProjectileMeshComponent->SetupAttachment(RootComponent);
    }
    // 3초 후 발사체를 제거합니다.
    InitialLifeSpan = 3.0f;
}

// 게임 시작 또는 스폰 시 호출
void AFPSProjectile::BeginPlay()
{
    Super::BeginPlay();

}

// 프레임마다 호출
void AFPSProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// 발사 방향으로의 발사체 속도를 초기화하는 함수입니다.
void AFPSProjectile::FireInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// 발사체가 어딘가에 부딪힐 때 호출되는 함수입니다.
void AFPSProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
    {
        OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
    }
    Destroy();
}