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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 RemainingTotalAmmo; //소지한 탄약
	//Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DryFireSound;

	FTimerHandle ReloadTimerHandle;
public:
	//getter
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmo() { return MaxAmmo; } const
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() { return CurrentAmmo; } const
	UFUNCTION(BlueprintCallable)
	int32 GetTotalAmmo() { return RemainingTotalAmmo; } const
	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() { return WeaponType; } const
	//setter
	UFUNCTION(BlueprintCallable)
	void SetMaxAmmo(int32 Amount) { MaxAmmo = Amount; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentAmmo(int32 Amount) { CurrentAmmo = Amount; }
	UFUNCTION(BlueprintCallable)
	void SetRemainingTotalAmmo(int32 Amount) { RemainingTotalAmmo = Amount; }
	UFUNCTION(BlueprintCallable)
	void SetWeaponType(EWeaponType Amount) { WeaponType = Amount; }



	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();  // 무기 발사 (오버라이드 가능)

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Reload();  // 탄약 재장전
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void ReloadAmmo();  // 탄약 재장전

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Equip();  // 무기 장착
};
