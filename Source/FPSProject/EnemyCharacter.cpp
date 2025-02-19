#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyCharacter::AEnemyCharacter()
{
    // AI Pawn 설정
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // 기본 값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 20.0f;
    AttackRange = 200.0f;
    DetectionRange = 1000.0f;
    bIsDead = false;

    // AI 이동 설정
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
                                class AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    // 인터페이스 구현 호출
    TakeDamage(ActualDamage);
    
    return ActualDamage;
}

// 인터페이스 구현
void AEnemyCharacter::TakeDamage(float DamageAmount)
{
    if (bIsDead) return;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AEnemyCharacter::Die()
{
    if (bIsDead) return;

    bIsDead = true;
    
    // 사망 애니메이션 재생
    if (DeathMontage)
    {
        PlayAnimation(DeathMontage);
    }

    // AI 이동 중지
    GetCharacterMovement()->StopMovementImmediately();
    
    // 콜리전 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 일정 시간 후 액터 제거
    SetLifeSpan(3.0f);
}

void AEnemyCharacter::Attack()
{
    if (bIsDead) return;

    if (AttackMontage)
    {
        PlayAnimation(AttackMontage);
    }
}

void AEnemyCharacter::MoveTo(FVector TargetLocation)
{
    if (bIsDead) return;

    // AI 컨트롤러를 통한 이동
    AController* AIController = GetController();
    if (AIController)
    {
        // 간단한 이동 구현
        SetActorLocation(FMath::VInterpTo(
            GetActorLocation(),
            TargetLocation,
            GetWorld()->GetDeltaSeconds(),
            2.0f
        ));
    }
}

void AEnemyCharacter::PlayAnimation(UAnimMontage* Animation)
{
    if (Animation)
    {
        PlayAnimMontage(Animation);
    }
}

bool AEnemyCharacter::IsAlive() const
{
    return !bIsDead;
}
