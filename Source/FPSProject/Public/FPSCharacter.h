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
	// ���� ���� (true = 1��Ī, false = 3��Ī)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "View", meta = (AllowPrivateAccess = "true"))
	bool bIsFirstPerson = true;

	// ĳ���� ü��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	// ĳ���Ͱ� ����ִ��� ����
	bool bIsAlive = true;

	// ��� �� ���� �ð� �� ������ ���� Ÿ�̸� �ڵ�
	FTimerHandle DeathTimerHandle;

public:
	// ������
	AFPSCharacter();

	// ���� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "View")
	void Viewpoint_Transformation();

	/** ũ���ġ(�����̵�) ���� */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartCrouch(const FInputActionValue& Value);

	/** ũ���ġ(�����̵�) ���� */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopCrouch(const FInputActionValue& Value);


	// ü�� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	float GetHealth() const;

	// �������� ���� �� ȣ��Ǵ� �Լ� (�������̽� ����)
	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	virtual void TakeDamage(float DamageAmount) override;

	// ĳ���� ��� ó�� �Լ� (�������̽� ����)
	virtual void Die() override;

	// ���� ���� �Լ� (�������̽� ����)
	virtual void Attack() override;

	// Ư�� ��ġ�� �̵��ϴ� �Լ� (�������̽� ����)
	virtual void MoveTo(FVector TargetLocation) override;

	// ĳ������ ���� ���θ� ��ȯ�ϴ� �Լ� (�������̽� ����)
	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override;

	// �ִϸ��̼��� �����ϴ� �Լ� (�������̽� ����)
	virtual void PlayAnimation(UAnimMontage* Animation) override;

protected:
	// ī�޶� �� ������ �� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// �̵� �ӵ� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	/** ��� �ִϸ��̼� */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	// �Է� ���ε��� ó���ϴ� �Լ�
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �Է� ó�� �Լ���
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

	// ĳ���� ���� �Լ�
	void DestroyCharacter();
};
