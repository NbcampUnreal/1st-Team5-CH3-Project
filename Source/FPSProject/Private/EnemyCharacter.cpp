#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.h"
#include "BasicGameInstance.h"
#include "GameFramework/DamageType.h"

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
    
    // 무기 위치 및 회전 조정 (공격 방향 수정)
    WeaponMesh->SetRelativeLocation(FVector(0.0f, -10.0f, 50.0f));
    WeaponMesh->SetRelativeRotation(FRotator(69.0f, -55.1f, 56.1f));
    
    // 물리 시뮬레이션을 위한 설정
    WeaponMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    WeaponMesh->SetSimulatePhysics(false); // 기본적으로는 비활성화
    WeaponMesh->SetEnableGravity(true);    // 중력 활성화
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    UpdateMovementSpeed();  // 초기 속도 설정
    
    // 무기 애니메이션 인스턴스 확인
    if (WeaponMesh)
    {
        UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
        if (WeaponAnimInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Instance is valid"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Instance is NULL - 에디터에서 무기 메시에 애니메이션 블루프린트를 설정해주세요"));
            
            // 스켈레탈 메시 정보 출력
            if (WeaponMesh->GetSkeletalMeshAsset())
            {
                UE_LOG(LogTemp, Warning, TEXT("Weapon Skeletal Mesh: %s"), 
                    *WeaponMesh->GetSkeletalMeshAsset()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Weapon Skeletal Mesh is not set"));
            }
        }
    }
    
    // 걷기 애니메이션 시작
    if (WeaponWalkMontage)
    {
        PlayWeaponAnimation(WeaponWalkMontage);
    }
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
        
        // 무기 물리 시뮬레이션 활성화
        if (WeaponMesh)
        {
            // 무기를 소켓에서 분리
            WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            
            // 물리 시뮬레이션 활성화
            WeaponMesh->SetSimulatePhysics(true);
            
            // 충돌 활성화
            WeaponMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
            
            // 캐릭터와 충돌하지 않도록 설정
            WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
            
            // 약간의 임펄스 추가하여 자연스럽게 떨어지도록 함
            WeaponMesh->AddImpulse(FVector(0.0f, 0.0f, -100.0f));
            
            // 약간의 회전도 추가
            WeaponMesh->AddAngularImpulseInDegrees(FVector(FMath::RandRange(-20.0f, 20.0f), 
                                                          FMath::RandRange(-20.0f, 20.0f), 
                                                          FMath::RandRange(-20.0f, 20.0f)));
            
            UE_LOG(LogTemp, Warning, TEXT("Weapon detached and physics simulation enabled"));
        }
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
        
        // 무기 공격 애니메이션도 재생
        if (WeaponAttackMontage)
        {
            PlayWeaponAnimation(WeaponAttackMontage);
        }
        
        // 무기 방향 조정 (플레이어를 향해 정확히 찌르도록)
        if (WeaponMesh && Player)
        {
            // 플레이어 방향으로 무기 회전 조정
            FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            FRotator DesiredRotation = DirectionToPlayer.Rotation();
            
            UE_LOG(LogTemp, Warning, TEXT("Attack started"));
        }
        
        // PlayRate를 고려한 실제 재생 시간 계산
        float ActualDuration = OriginalDuration / PlayRate;
        
        // 공격 데미지 적용 타이머 (애니메이션의 중간 지점에서)
        FTimerHandle DamageTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DamageTimerHandle,
            [this, Player]() {
                // 플레이어가 유효하고 살아있는지 확인
                AFPSCharacter* FPSPlayer = Cast<AFPSCharacter>(Player);
                if (FPSPlayer && FPSPlayer->IsAlive())
                {
                    // 거리 계산
                    float DistToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
                    
                    // 공격 방향 벡터
                    FVector AttackDirection = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                    
                    // 캐릭터의 전방 벡터와 공격 방향 벡터의 내적 계산 (각도 확인)
                    float DotProduct = FVector::DotProduct(GetActorForwardVector(), AttackDirection);
                    
                    // 거리가 공격 범위 내이고, 내적이 0.7 이상(약 45도 이내)이면 데미지 적용
                    if (DistToPlayer <= AttackRange && DotProduct >= 0.7f)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Attack hit player! Applying damage: %f"), WeaponDamage);
                        FPSPlayer->TakeDamage(WeaponDamage);
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Attack missed! Distance: %f, Angle: %f"), 
                            DistToPlayer, FMath::Acos(DotProduct) * 180.0f / PI);
                    }
                }
            },
            ActualDuration * 0.5f, // 애니메이션의 50% 지점에서 데미지 계산
            false
        );
        
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
                            
                            // 걷기 애니메이션 재생
                            if (WeaponWalkMontage)
                            {
                                PlayWeaponAnimation(WeaponWalkMontage);
                            }
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

// 무기 애니메이션 재생 함수 추가
void AEnemyCharacter::PlayWeaponAnimation(UAnimMontage* WeaponAnimation)
{
    if (WeaponAnimation && WeaponMesh)
    {
        // 무기 메시의 애니메이션 인스턴스 가져오기
        UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
        if (WeaponAnimInstance)
        {
            float Duration = WeaponAnimInstance->Montage_Play(WeaponAnimation);
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Duration: %f"), Duration);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Weapon Animation Instance is null!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Weapon Animation or WeaponMesh is null!"));
    }
}
