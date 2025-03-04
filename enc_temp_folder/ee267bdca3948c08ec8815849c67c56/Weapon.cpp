#include "Weapon.h"
#include "kismet/GameplayStatics.h"
#include "BasicGameState.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	RemainingTotalAmmo = 20;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void AWeapon::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("우리리"));

	if (CurrentAmmo > 0)
	{
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				FireSound,
				GetActorLocation()
			);
		}
		UE_LOG(LogTemp, Warning, TEXT("Fire Ammo"));
		CurrentAmmo--;
	}
	else
	{
		if (DryFireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				DryFireSound,
				GetActorLocation()
			);
		}
	}
}

void AWeapon::Reload()
{
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ReloadSound,
			GetActorLocation()
		);
	}

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&AWeapon::ReloadAmmo,
		1.0f,
		false
	);
}

void AWeapon::ReloadAmmo()
{
	int32 ReloadCount = std::min(MaxAmmo - CurrentAmmo, RemainingTotalAmmo);
	RemainingTotalAmmo -= ReloadCount;
	CurrentAmmo += ReloadCount;

	ABasicGameState* BasicGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(this));
	if (BasicGameState)
	{
		BasicGameState->UpdateAmmoHUD();
	}
}

void AWeapon::Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Equipped"), *GetName());
}
