#include "BulletItem.h"
#include "FPSCharacter.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

ABulletItem::ABulletItem()
{
	ItemType = "Bullet"; // ������ Ÿ�� ����
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	AFPSCharacter* Player = Cast<AFPSCharacter>(Activator);
	if (Player)
	{
		AWeapon* CurrentWeapon = Player->GetCurrentWeapon(); // ���� ���� ��������
		if (CurrentWeapon)
		{
			CurrentWeapon->SetRemainingTotalAmmo(CurrentWeapon->GetTotalAmmo() + AmmoAmount); // �Ѿ� ����
			UE_LOG(LogTemp, Log, TEXT("Bullet Item Used: %d Ammo Added"), AmmoAmount);
		}
	}

	

	// ������ ����
	DestroyItem();
}
