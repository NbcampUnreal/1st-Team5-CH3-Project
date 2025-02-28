#include "SimWeapon2.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.h"
#include "EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"

ASimWeapon2::ASimWeapon2()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);
}

void ASimWeapon2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwner());
    if (!Character) return;

    UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
    if (!CameraComp) return;

    FVector Start = CameraComp->GetComponentLocation();
    FVector ForwardVector = CameraComp->GetForwardVector();
    FVector End = Start + (ForwardVector * FireRange);

    DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.01f, 0, 2.0f);
}

void ASimWeapon2::Fire()
{
    UE_LOG(LogTemp, Warning, TEXT("SimWeapon2: Fire!"));

    AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwner());
    if (!Character) return;

    UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
    if (!CameraComp) return;

    FVector Start = CameraComp->GetComponentLocation();
    FVector ForwardVector = CameraComp->GetForwardVector();
    FVector End = Start + (ForwardVector * FireRange);

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);

    DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 2.0f);

    if (bHit)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit something: %s"), *HitActor->GetName());

            AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(HitActor);
            if (Enemy)
            {
                UE_LOG(LogTemp, Warning, TEXT("Hit enemy! Applying damage: %f"), DamageAmount);
                Enemy->TakeDamage(DamageAmount);  
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Hit a non-enemy target."));
            }

            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 2.0f);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SimWeapon2: Missed the target!"));
    }

    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("MuzzleFlashSocket"));
    }
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }
}
