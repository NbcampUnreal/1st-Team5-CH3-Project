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
	// 현재 시점 (true = 1인칭, false = 3인칭)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "View", meta = (AllowPrivateAccess = "true"))
	bool bIsFirstPerson = true;

	// 캐릭터 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Stats", meta = (AllowPrivateAccess = "true"))
	float Health;

	// 캐릭터가 살아있는지 여부
	bool bIsAlive = true;

	// 사망 후 일정 시간 후 삭제를 위한 타이머 핸들
	FTimerHandle DeathTimerHandle;

public:
	// 생성자
	AFPSCharacter();

	// 시점 변경 함수
	UFUNCTION(BlueprintCallable, Category = "View")
	void Viewpoint_Transformation();

	/** 크라우치(슬라이딩) 시작 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartCrouch(const FInputActionValue& Value);

	/** 크라우치(슬라이딩) 종료 */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopCrouch(const FInputActionValue& Value);


	// 체력 관련 함수
	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	float GetHealth() const;

	// 데미지를 받을 때 호출되는 함수 (인터페이스 구현)
	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	virtual void TakeDamage(float DamageAmount) override;

	// 캐릭터 사망 처리 함수 (인터페이스 구현)
	virtual void Die() override;

	// 공격 동작 함수 (인터페이스 구현)
	virtual void Attack() override;

	// 특정 위치로 이동하는 함수 (인터페이스 구현)
	virtual void MoveTo(FVector TargetLocation) override;

	// 캐릭터의 생존 여부를 반환하는 함수 (인터페이스 구현)
	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override;

	// 애니메이션을 실행하는 함수 (인터페이스 구현)
	virtual void PlayAnimation(UAnimMontage* Animation) override;

protected:
	// 카메라 및 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 이동 속도 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	/** 사망 애니메이션 */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

	// 입력 바인딩을 처리하는 함수
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 입력 처리 함수들
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

	// 캐릭터 삭제 함수
	void DestroyCharacter();
};
