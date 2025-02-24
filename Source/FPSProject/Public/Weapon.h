#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	StunGun UMETA(DisplayName = "Stun Gun"),
	AggroTool UMETA(DisplayName = "Aggro Tool"),
	NormalGun UMETA(DisplayName = "Normal Gun")
};

UCLASS()
class FPSPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;  // 무기 타입

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;  // 최대 탄약 수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo;  // 현재 탄약

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();  // 무기 발사 (오버라이드 가능)

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();  // 탄약 재장전

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip();  // 무기 장착
};
