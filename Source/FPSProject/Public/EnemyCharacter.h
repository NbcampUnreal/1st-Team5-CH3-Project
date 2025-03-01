#pragma once

class AEnemyAIController;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "FPSCharacter.h"
#include "Components/WidgetComponent.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class FPSPROJECT_API AEnemyCharacter : public ACharacter, public ICharacterInterface
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

    // AActor의 TakeDamage 오버라이드
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
    
    // 무기 애니메이션 재생 함수
    void PlayWeaponAnimation(UAnimMontage* WeaponAnimation);
    
    // 플레이어 감지 상태 설정
    UFUNCTION(BlueprintCallable, Category = "AI|Detection")
    void SetPlayerDetected(bool bDetected);
    
    // 플레이어 감지 상태 반환
    UFUNCTION(BlueprintCallable, Category = "AI|Detection")
    bool IsPlayerDetected() const { return bPlayerDetected; }
    
    // 수면 타이머 업데이트
    UFUNCTION(BlueprintCallable, Category = "AI|State")
    void UpdateSleepTimer(float RemainingTime);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

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

    // 무기 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Weapon")
    UAnimMontage* WeaponAttackMontage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation|Weapon")
    UAnimMontage* WeaponWalkMontage;

    // 피격 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* HitReactionMontage;
    
    // 피격 사운드
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* HitSound;

    // 공격 관련 변수
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
    
    // 헤더에 상태별 감지 범위 수정자 추가
    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float BaseDetectionRange = 600.0f;  // 기존 DetectionRange 값을 기본값으로

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float SprintingRangeMultiplier = 1.5f;

    UPROPERTY(EditAnywhere, Category = "AI|Detection")
    float CrouchingRangeMultiplier = 0.5f;

    // 무기 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USkeletalMeshComponent* WeaponMesh;
    
    // 무기 데미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float WeaponDamage = 20.0f;

    // 감지 범위 UI 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* DetectionRangeWidgetComp;
    
    // 수면 상태 UI 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    UWidgetComponent* SleepStateWidgetComp;
    
    // 플레이어 감지 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Detection")
    bool bPlayerDetected;
    
    // 수면 상태 지속 시간
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    float SleepDuration;
    
    // 수면 상태 남은 시간
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    float SleepRemainingTime;

private:
    // 사망 상태
    bool bIsDead;

    // 마취 해제 함수
    void WakeUp();
};
