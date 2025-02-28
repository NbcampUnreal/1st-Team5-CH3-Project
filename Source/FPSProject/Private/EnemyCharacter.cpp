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

    // 무기 메시 컴포넌트 생성
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(GetMesh(), "WeaponSocket_L");
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
    UE_LOG(LogTemp, Warning, TEXT("Enemy took damage: %f, Current Health: %f"), DamageAmount, CurrentHealth);
    
    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AEnemyCharacter::Die()
{
    if (bIsDead) return;
    bIsDead = true;
    
    // 킬 카운트 증가
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
    {
        if (UBasicGameInstance* BasicGameInstance = Cast<UBasicGameInstance>(GameInstance))
        {
            BasicGameInstance->AddKill();     // 킬 카운트만 증가
            UE_LOG(LogTemp, Warning, TEXT("Enemy died! Kill Count: %d"), BasicGameInstance->TotalKillCount);
        }
    }
    
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

    // 플레이어 방향으로 회전
    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Player)
    {
        FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }

    // 이동 중지
    GetCharacterMovement()->StopMovementImmediately();

    // 공격 애니메이션 재생
    if (AttackMontage)
    {
        float PlayRate = 2.5f;
        // 원래 애니메이션 길이를 가져옴
        float OriginalDuration = PlayAnimMontage(AttackMontage, PlayRate);
        
        // PlayRate를 고려한 실제 재생 시간 계산
        float ActualDuration = OriginalDuration / PlayRate;

        // 실제 재생 시간으로 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(
            AttackTimerHandle,
            [this]()
            {
                // AI 컨트롤러의 행동 재개
                if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
                {
                    // 행동 로직 재개
                    AIController->GetBrainComponent()->StartLogic();
                    
                    // 현재 행동 트리 재시작
                    AIController->GetBrainComponent()->RestartLogic();
                    
                    // 플레이어를 향해 즉시 이동 명령
                    if (AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
                    {
                        float DistToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
                        if (DistToPlayer > AttackRange)
                        {
                            AIController->MoveToActor(Player);
                            bIsChasing = true;
                            UpdateMovementSpeed();
                        }
                    }
                }
            },
            ActualDuration * 0.9f,
            false
        );

        // AI 컨트롤러의 행동 중지
        if (AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController()))
        {
            AIController->GetBrainComponent()->StopLogic("Attacking");
        }
    }

    // 공격 쿨다운 설정
    bCanAttack = false;
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        [this]() { bCanAttack = true; },
        AttackCooldown,
        false
    );

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
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
