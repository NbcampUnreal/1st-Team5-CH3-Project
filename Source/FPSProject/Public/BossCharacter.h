#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "BossCharacter.generated.h"

UCLASS()
class FPSPROJECT_API ABossCharacter : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    // 생성자
    ABossCharacter();

    // 초기화 함수
    virtual void BeginPlay() override;

    // Tick 함수
    virtual void Tick(float DeltaTime) override;

    // 데미지 처리 함수 오버라이드
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                             class AController *EventInstigator, AActor *DamageCauser) override;

    // 인터페이스 TakeDamage 함수 오버라이드
    virtual void TakeDamage(float DamageAmount) override;

    // 수면 함수 오버라이드
    virtual void Sleep(float Duration) override;
    
    // 깨어나기 함수 오버라이드
    virtual void WakeUp() override;

    // 공격 범위 체크 함수
    bool IsInAttackRange(AActor *Target) const;

    // 이동 속도 업데이트 함수 오버라이드
    virtual void UpdateMovementSpeed();

    // 공격 함수 오버라이드 - 공격 속도 배율 적용
    virtual void Attack() override;

    // 무기 애니메이션 재생 함수 오버로드
    void PlayWeaponAnimation(UAnimMontage* WeaponAnimation, float PlayRate);

    // 보스 전용 추가 공격 패턴 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
    void SpecialAttack();

    // 일반 공격 함수
    void NormalAttack();

    // 특수 공격의 각 단계를 실행하는 헬퍼 함수
    void ExecuteSpecialAttack(AActor* Player, int32 AttackIndex, int32 TotalAttacks, float SpecialPlayRate, float SpecialDamage);

    // 감지 기능 활성화/비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|Detection")
    void SetDetectionEnabled(bool bEnabled);

    // 감지 UI 표시/숨김 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|Detection")
    void SetDetectionUIVisible(bool bVisible);

    // 감지 기능 상태 확인 함수
    UFUNCTION(BlueprintPure, Category = "Boss|Detection")
    bool IsDetectionEnabled() const { return bDetectionEnabled; }

    // 순찰 모드 설정 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|AI")
    void SetPatrolMode(bool bEnabled);

    // 순찰 모드 상태 확인 함수
    UFUNCTION(BlueprintPure, Category = "Boss|AI")
    bool IsInPatrolMode() const { return bPatrolMode; }

    // 보스 체력 UI 표시 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|UI")
    void ShowBossHealthUI();

    // 보스 체력 UI 숨김 함수
    UFUNCTION(BlueprintCallable, Category = "Boss|UI")
    void HideBossHealthUI();

    // 보스 체력 비율 반환 함수 (0.0-1.0)
    UFUNCTION(BlueprintPure, Category = "Boss|Stats")
    float GetHealthPercent() const;

    // 보스 체력 UI가 표시 중인지 확인
    UFUNCTION(BlueprintPure, Category = "Boss|UI")
    bool IsHealthUIVisible() const { return bHealthUIVisible; }

    // 사망 처리 함수 (부모 클래스의 함수를 오버라이드)
    virtual void Die() override;

private:
    // 보스 스케일 - 기본 Enemy보다 크기
    UPROPERTY(EditDefaultsOnly, Category = "Boss|Appearance")
    float BossScale;

    // 공격 속도 배율 - 기본 Enemy보다 빠름
    UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat")
    float AttackSpeedMultiplier;

    // 이동 속도 배율 - 기본 Enemy보다 빠름
    UPROPERTY(EditDefaultsOnly, Category = "Boss|Movement")
    float MovementSpeedMultiplier;

    // 감지 기능 활성화 여부
    UPROPERTY(EditAnywhere, Category = "Boss|Detection")
    bool bDetectionEnabled;

    // 순찰 모드 여부
    UPROPERTY(EditAnywhere, Category = "Boss|AI")
    bool bPatrolMode;

    // 체력 UI 표시 여부
    UPROPERTY(VisibleAnywhere, Category = "Boss|UI")
    bool bHealthUIVisible;

    // 체력 UI 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Boss|UI")
    TSubclassOf<UUserWidget> HealthUIWidgetClass;

    // 체력 UI 위젯 인스턴스
    UPROPERTY()
    UUserWidget* HealthUIWidget;
};