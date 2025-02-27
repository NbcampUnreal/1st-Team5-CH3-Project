#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.h"
#include "BasicGameInstance.h"

AEnemyCharacter::AEnemyCharacter()
{
    // AI Pawn 설정
    AIControllerClass = AEnemyAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    
    // 기본 값 설정
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    AttackDamage = 20.0f;
    AttackRange = 300.0f;     // 공격 범위
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
    if (bIsDead || !bCanAttack) return;

    // 플레이어 위치 확인 및 회전
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Player)
    {
        // 플레이어 방향으로 즉시 회전
        FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }

    // 공격 쿨다운 설정
    bCanAttack = false;
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        [this]() { bCanAttack = true; },
        AttackCooldown,  // EnemyCharacter.h에 이미 정의된 변수 (기본값 2.0f)
        false
    );

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
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
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
    DrawDebugLine(GetWorld(), MuzzlePos, MuzzlePos + Direction * 10000.0f, FColor::Cyan, false, 1.0f, 0, 1.0f);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, MuzzlePos, MuzzlePos + Direction * 10000.0f, ECC_Pawn, QueryParams))
    {
        UE_LOG(LogTemp, Warning, TEXT("Hit something: %s"), *HitResult.GetActor()->GetName());
        
        if (AFPSCharacter* FPSPlayer = Cast<AFPSCharacter>(HitResult.GetActor()))
        {
            // 인터페이스를 통해 데미지 전달
            ICharacterInterface* CharacterInterface = Cast<ICharacterInterface>(FPSPlayer);
            if (CharacterInterface)
            {
                CharacterInterface->TakeDamage(AttackDamage);
                UE_LOG(LogTemp, Warning, TEXT("Hit FPSPlayer! Applying damage: %f"), AttackDamage);
            }
            
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
    AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
    if (bIsDead || (AIController && AIController->IsInAttackRange())) return;

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

void AEnemyCharacter::Sleep(float Duration)
{
    if (bIsDead || bIsSleeping) return;

    bIsSleeping = true;
    
    // AI 이동 중지
    GetCharacterMovement()->StopMovementImmediately();
    
    // AI 컨트롤러 비활성화
    if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StopLogic("Sleeping");
    }

    // 게임 인스턴스에 수면 카운트 증가
    if (UBasicGameInstance* GameInstance = Cast<UBasicGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->AddSleep();
    }

    // 지정된 시간 후에 깨어나기
    GetWorld()->GetTimerManager().SetTimer(
        SleepTimerHandle,
        this,
        &AEnemyCharacter::WakeUp,
        Duration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Enemy is now sleeping for %f seconds"), Duration);
}

void AEnemyCharacter::WakeUp()
{
    if (bIsDead) return;

    bIsSleeping = false;
    
    // AI 컨트롤러 다시 활성화
    if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StartLogic();
    }

    UE_LOG(LogTemp, Warning, TEXT("Enemy woke up from sleep"));
}

void AEnemyCharacter::UpdateDetectionRangeForPlayerState(AFPSCharacter* Player)
{
    if (!Player) return;

    ECharacterState CurrentPlayerState = Player->GetCurrentState();
    
    switch (CurrentPlayerState)
    {
        case ECharacterState::Sprinting:
            DetectionRange = 900.0f;  // 달리기: 기본 범위의 1.5배
            break;
            
        case ECharacterState::Crouching:
            DetectionRange = 300.0f;  // 앉기: 기본 범위의 0.5배
            break;
            
        case ECharacterState::Dead:
            DetectionRange = 0.0f;    // 사망: 감지 안함
            break;
            
        default:  // Normal 상태
            DetectionRange = 600.0f;  // 걷기: 기본 감지 범위
            break;
    }
}
