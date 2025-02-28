#pragma once

class AEnemyAIController;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
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

    // 마취 상태인지 확인
    UFUNCTION(BlueprintCallable, Category = "AI|State")
    bool IsSleeping() const { return bIsSleeping; }

    // 마취 효과 적용 (마취총에 맞았을 때 호출)
    UFUNCTION(BlueprintCallable, Category = "AI|State")
    void Sleep(float Duration);

    // 플레이어 상태에 따라 감지 범위 업데이트
    void UpdateDetectionRangeForPlayerState(AFPSCharacter* Player);
    
    // 무기 충돌 감지 함수
    UFUNCTION()
    void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                        bool bFromSweep, const FHitResult& SweepResult);
                        
    // 무기 충돌 활성화/비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    void SetWeaponCollisionEnabled(bool bEnabled);
    
    // 디버그 시각화 함수
    void DrawDebugWeaponCollision();

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

    // 사망 애니메이션
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathMontage;

    // protected 섹션에 추가
    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float PatrolSpeed = 200.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float ChaseSpeed = 400.0f;

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

    // 마취 상태
    UPROPERTY(VisibleAnywhere, Category = "AI|State")
    bool bIsSleeping;

    // 마취 해제 타이머
    FTimerHandle SleepTimerHandle;

    bool bCanAttack = true;
    FTimerHandle AttackCooldownTimer;
    FTimerHandle AttackTimerHandle;
    
    // 디버그 시각화 타이머
    FTimerHandle DebugTimerHandle;

    // 헤더에 상태별 감지 범위 수정자 추가
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float BaseDetectionRange = 600.0f;  // 기존 DetectionRange 값을 기본값으로

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float SprintingRangeMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float CrouchingRangeMultiplier = 0.5f;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    USkeletalMeshComponent* WeaponMesh;
    
    // 무기 끝에 부착할 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    UBoxComponent* WeaponTipCollision;
    
    // 무기 데미지 값
    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float WeaponDamage = 10.0f;
    
    // 무기 충돌 활성화 상태
    bool bWeaponCollisionEnabled = false;

private:
    // 사망 상태
    bool bIsDead;

    // 마취 해제 함수
    void WakeUp();
};
