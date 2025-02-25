#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
    // AI Pawn 설정
    AIControllerClass = AEnemyAIController::StaticClass();
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
    
     // AI 컨트롤러와의 연결 끊기
    if (AController* AIController = GetController())
    {
        AIController->UnPossess();
    }
    
    // 콜리전 비활성화
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 일정 시간 후 액터 제거
    SetLifeSpan(3.0f);
}

void AEnemyCharacter::Attack()
{
    if (bIsDead) return;

    if (ShootMontage)
    {
        PlayAnimation(ShootMontage);
        UE_LOG(LogTemp, Warning, TEXT("Playing Shoot Montage"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ShootMontage is not set!"));
    }
}

void AEnemyCharacter::MoveTo(FVector TargetLocation)
{
    if (bIsDead) return;

    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (AIController)
    {
        // AI 네비게이션 시스템을 사용한 이동
        AIController->MoveToLocation(TargetLocation, -1.0f);
        UE_LOG(LogTemp, Warning, TEXT("Moving to location: %s"), *TargetLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("AI Controller is not valid!"));
    }
}

void AEnemyCharacter::PlayAnimation(UAnimMontage* Animation)
{
    if (Animation)
    {
        float Duration = PlayAnimMontage(Animation);
        UE_LOG(LogTemp, Warning, TEXT("Animation Duration: %f"), Duration);
        
        // 스켈레톤 정보 출력
        if (GetMesh() && GetMesh()->GetSkeletalMeshAsset())
        {
            UE_LOG(LogTemp, Warning, TEXT("Character Skeleton: %s"), 
                *GetMesh()->GetSkeletalMeshAsset()->GetSkeleton()->GetName());
        }
        if (Animation->GetSkeleton())
        {
            UE_LOG(LogTemp, Warning, TEXT("Montage Skeleton: %s"), 
                *Animation->GetSkeleton()->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Animation is null!"));
    }
}

bool AEnemyCharacter::IsAlive() const
{
    return !bIsDead;
}
