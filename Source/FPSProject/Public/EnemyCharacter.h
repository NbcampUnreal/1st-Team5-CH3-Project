#pragma once

class AEnemyAIController;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FPSCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class FPSPROJECT_API AEnemyCharacter : public ACharacter, public ICharacterInterface
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    // AActor의 TakeDamage 오버라이드드
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
                           class AController* EventInstigator, AActor* DamageCauser) override;

    // CharacterInterface 구현
    virtual void TakeDamage(float DamageAmount) override;
    virtual void Die() override;
    virtual void Attack() override;
    virtual void MoveTo(FVector TargetLocation) override;
    virtual void PlayAnimation(UAnimMontage* Animation) override;
    virtual bool IsAlive() const override;

    // 공격 범위 반환
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    float GetAttackRange() const { return AttackRange; }

    // 감지 범위 반환
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    float GetDetectionRange() const { return DetectionRange; }

    // 애니메이션 상태를 public으로 이동
    UPROPERTY(BlueprintReadWrite, Category = "Animation")
    bool bIsChasing = false;

    // 이동 속도 업데이트
    void UpdateMovementSpeed();

protected:
    virtual void BeginPlay() override;

    // 현재 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
    float CurrentHealth;

    // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Stats")
    float MaxHealth;

    // 공격력 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackDamage;

    // 공격 범위 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float AttackRange;

    // 감지 범위 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
    float DetectionRange;

    // 총쏘기 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* ShootMontage;

    // 사망 애니메이션
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathMontage;

    // protected 섹션에 추가
    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float PatrolSpeed = 200.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float ChaseSpeed = 400.0f;

    // 원거리 공격 관련 변수 추가
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float ProjectileSpeed = 2000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<class AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    USceneComponent* MuzzleLocation;

    // 히트 이펙트
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    UParticleSystem* HitEffect;

    // 공격 사운드
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* AttackSound;

    // 공격 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* AttackMontage;

    // 공격 관련 변수들
    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackCooldown = 2.0f;    // 기본 공격 쿨타임

private:
    // 사망 상태
    bool bIsDead;

    // 원거리 공격 함수 (나중에 구현할 예정)
    void FireProjectile();

    // 공격 관련 변수
    FTimerHandle AttackTimerHandle;
    bool bCanAttack = true;
};
