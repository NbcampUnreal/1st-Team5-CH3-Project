#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Weapon.h" 
#include "FPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

// AI 감지 시스템을 위한 캐릭터 상태 정의
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Normal      UMETA(DisplayName = "Normal State"),
	Sprinting   UMETA(DisplayName = "Sprinting State"),
	Crouching   UMETA(DisplayName = "Crouching State"),
	Dead        UMETA(DisplayName = "Dead State")
};

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter, public ICharacterInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "View", meta = (AllowPrivateAccess = "true"))
	bool bIsFirstPerson = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	bool bIsAlive = true;

	FTimerHandle DeathTimerHandle;

public:
	AFPSCharacter();

	UFUNCTION(BlueprintCallable, Category = "View")
	void Viewpoint_Transformation();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartCrouch(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopCrouch(const FInputActionValue& Value);


	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	virtual void TakeDamage(float DamageAmount) override;

	virtual void Die() override;

	virtual void Attack() override;

	virtual void MoveTo(FVector TargetLocation) override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override;

	virtual void PlayAnimation(UAnimMontage* Animation) override;

	// AI가 캐릭터의 현재 상태를 확인하기 위한 getter
	UFUNCTION(BlueprintCallable, Category = "Character State")
	ECharacterState GetCurrentState() const { return CurrentState; }

	// 캐릭터 상태 변경 시 호출 (Sprint, Crouch 등의 함수에서 자동 호출)
	UFUNCTION(BlueprintCallable, Category = "Character State")
	void SetCharacterState(ECharacterState NewState);

	// 현재 장착된 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* CurrentWeapon;

	// 무기 배열 (다양한 무기를 저장)
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<TSubclassOf<AWeapon>> WeaponClasses;

	// 현재 장착된 1인칭(FPS) 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* CurrentWeapon1P;

	// 현재 장착된 3인칭(TPS) 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	AWeapon* CurrentWeapon3P;


	// 무기 장착 함수
	void EquipWeapon(int32 WeaponIndex);

	// 무기 변경 함수 (입력 바인딩용)
	void SelectWeapon1();
	void SelectWeapon2();
	// **총 발사 요청**
	void Fire();
	// 재장전
	void Reload();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* FirstPersonMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void StartJump(const FInputActionValue& value);

	UFUNCTION()
	void StopJump(const FInputActionValue& value);

	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	void DestroyCharacter();

	// AI 감지 시스템에서 참조할 현재 캐릭터 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character State")
	ECharacterState CurrentState = ECharacterState::Normal;

	// 상태 변경 시 블루프린트에서 처리할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Character State")
	void OnStateChanged(ECharacterState NewState);

	// 상태 변경 시 C++에서 처리할 로직
	UFUNCTION()
	virtual void HandleStateChange(ECharacterState NewState);
};
