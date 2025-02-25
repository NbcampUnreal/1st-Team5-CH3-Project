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
    AttackRange = 400.0f;     // 공격 범위 
    DetectionRange = 600.0f;  // 감지 범위 
    bIsDead = false;

    // AI 이동 설정
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    UpdateMovementSpeed();  // 초기 속도 설정
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

    // 원거리 공격 구현
    FireProjectile();
    
    // 공격 사운드 재생
    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

void AEnemyCharacter::FireProjectile()
{
    // 플레이어 위치 확인
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;
    
    // 발사 위치 및 방향 계산
    FVector MuzzlePos = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0, 0, 50.0f);
    
    // 메시에 소켓이 있으면 소켓 위치 사용
    if (GetMesh()->DoesSocketExist(FName("MuzzleSocket")))
    {
        MuzzlePos = GetMesh()->GetSocketLocation(FName("MuzzleSocket"));
    }
    
    FVector Direction = (Player->GetActorLocation() - MuzzlePos).GetSafeNormal();
    
    // 약간의 오차 추가 (완벽한 조준 방지)
    float Spread = 0.05f;
    Direction = FMath::VRandCone(Direction, Spread);
    
    // 히트스캔 방식으로 즉시 데미지 적용
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    // 디버그 라인 그리기 (개발 중에만)
    DrawDebugLine(GetWorld(), MuzzlePos, MuzzlePos + Direction * 10000.0f, FColor::Red, false, 1.0f, 0, 1.0f);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, MuzzlePos, MuzzlePos + Direction * 10000.0f, ECC_Pawn, QueryParams))
    {
        if (HitResult.GetActor() == Player)
        {
            UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());
            UE_LOG(LogTemp, Warning, TEXT("Enemy hit player with attack for %f damage"), AttackDamage);
            
            // 히트 이펙트 (선택적)
            if (HitEffect)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitResult.Location, FRotator::ZeroRotator, true);
            }
        }
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

// bIsChasing이 변경될 때 속도를 업데이트하는 함수 추가
void AEnemyCharacter::UpdateMovementSpeed()
{
    float NewSpeed = bIsChasing ? ChaseSpeed : PatrolSpeed;
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}
