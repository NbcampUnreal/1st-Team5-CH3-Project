#include "SimWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.h"
#include "EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"

ASimWeapon::ASimWeapon()
{
    PrimaryActorTick.bCanEverTick = true;  // Tick 함수 활성화 (조준선 갱신)
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);
}

// 총알이 날아갈 경로를 표시하는 조준선
void ASimWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwner());
    if (!Character) return;

    UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
    if (!CameraComp) return;

    // 조준 방향 계산
    FVector Start = CameraComp->GetComponentLocation();
    FVector ForwardVector = CameraComp->GetForwardVector();
    FVector End = Start + (ForwardVector * FireRange);

    // 조준 궤적을 시각적으로 표시 (초록색)
    DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.01f, 0, 2.0f);
}

void ASimWeapon::Fire()
{
    UE_LOG(LogTemp, Warning, TEXT("SimWeapon: Fire!"));

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

    // 발사 시 기존 조준 궤적을 빨간색으로 변경
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
                UE_LOG(LogTemp, Warning, TEXT("Tranquilizer hit! Enemy is put to sleep."));
                Enemy->Sleep(5.0f);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Hit a non-enemy target."));
            }

            // 맞은 지점을 노란색 점으로 표시
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 2.0f);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SimWeapon: Missed the target!"));
    }

    // 총구 이펙트 및 사운드 추가
    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("MuzzleFlashSocket"));
    }
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }
}
