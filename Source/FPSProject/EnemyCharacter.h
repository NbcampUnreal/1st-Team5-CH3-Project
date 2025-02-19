#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
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

    // 공격 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AttackMontage;

    // 사망 애니메이션
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathMontage;

private:
    // 사망 상태
    bool bIsDead;
};
