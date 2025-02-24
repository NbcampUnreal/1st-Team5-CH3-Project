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

public:
    AFPSCharacter();

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

    // ICharacterInterface implementation
    virtual void TakeDamage(float DamageAmount) override;
    virtual void Die() override;
    virtual void Attack() override;
    virtual void MoveTo(FVector TargetLocation) override;
    virtual void PlayAnimation(UAnimMontage* Animation) override;
    virtual bool IsAlive() const override;
};
