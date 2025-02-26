#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "FPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

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

protected:
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
};
