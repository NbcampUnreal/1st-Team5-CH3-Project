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

	bIsReloading = false;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}


void AWeapon::Fire()
{
	if (!bIsReloading)
	{
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
}

void AWeapon::Reload()
{
	bIsReloading = true;
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

	bIsReloading = false;
}

void AWeapon::Equip()
{
	UE_LOG(LogTemp, Warning, TEXT("%s Equipped"), *GetName());
}
