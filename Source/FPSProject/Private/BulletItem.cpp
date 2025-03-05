#include "BulletItem.h"
#include "FPSCharacter.h"
#include "BasicGameState.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

ABulletItem::ABulletItem()
{
	ItemType = "Bullet"; 
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	AFPSCharacter* Player = Cast<AFPSCharacter>(Activator);
	if (Player)
	{
		AWeapon* CurrentWeapon = Player->GetCurrentWeapon(); 
		if (CurrentWeapon)
		{
			CurrentWeapon->SetRemainingTotalAmmo(CurrentWeapon->GetTotalAmmo() + AmmoAmount); // �Ѿ� ����
		}
		// HUD Update
		ABasicGameState* BasicGameState = Cast<ABasicGameState>(UGameplayStatics::GetGameState(this));
		if (BasicGameState)
		{
			BasicGameState->UpdateAmmoHUD();
		}
	}

	DestroyItem();
}
