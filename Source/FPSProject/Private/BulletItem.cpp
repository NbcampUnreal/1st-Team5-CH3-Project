#include "BulletItem.h"
#include "FPSCharacter.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"

ABulletItem::ABulletItem()
{
	ItemType = "Bullet"; // 아이템 타입 설정
}

void ABulletItem::ActivateItem(AActor* Activator)
{
	AFPSCharacter* Player = Cast<AFPSCharacter>(Activator);
	if (Player)
	{
		AWeapon* CurrentWeapon = Player->GetCurrentWeapon(); // 현재 무기 가져오기
		if (CurrentWeapon)
		{
			CurrentWeapon->SetRemainingTotalAmmo(CurrentWeapon->GetTotalAmmo() + AmmoAmount); // 총알 증가
			UE_LOG(LogTemp, Log, TEXT("Bullet Item Used: %d Ammo Added"), AmmoAmount);
		}
	}

	

	// 아이템 제거
	DestroyItem();
}
