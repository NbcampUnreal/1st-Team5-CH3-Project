#include "SimWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.h"
#include "EnemyCharacter.h"
#include "Components/SkeletalMeshComponent.h"

ASimWeapon::ASimWeapon()
{
    PrimaryActorTick.bCanEverTick = true;  // Tick �Լ� Ȱ��ȭ (���ؼ� ����)
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);
}

// �Ѿ��� ���ư� ��θ� ǥ���ϴ� ���ؼ�
void ASimWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwner());
    if (!Character) return;

    UCameraComponent* CameraComp = Character->FindComponentByClass<UCameraComponent>();
    if (!CameraComp) return;

    // ���� ���� ���
    FVector Start = CameraComp->GetComponentLocation();
    FVector ForwardVector = CameraComp->GetForwardVector();
    FVector End = Start + (ForwardVector * FireRange);

    // ���� ������ �ð������� ǥ�� (�ʷϻ�)
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

    // �߻� �� ���� ���� ������ ���������� ����
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

            // ���� ������ ����� ������ ǥ��
            DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Yellow, false, 2.0f);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SimWeapon: Missed the target!"));
    }

    // �ѱ� ����Ʈ �� ���� �߰�
    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, WeaponMesh, TEXT("MuzzleFlashSocket"));
    }
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }
}
