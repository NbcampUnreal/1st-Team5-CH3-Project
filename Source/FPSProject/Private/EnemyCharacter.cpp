#include "EnemyCharacter.h"
#include "FPSCharacter.h"
#include "BossCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
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
    AttackRange = 300.0f;    // 공격 범위
    DetectionRange = 800.0f; // 감지 범위
    bIsDead = false;
    bPlayerDetected = false;
    SleepDuration = 0.0f;
    SleepRemainingTime = 0.0f;

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

    // 감지 범위 UI 위젯 컴포넌트 생성
    DetectionRangeWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionRangeWidget"));
    DetectionRangeWidgetComp->SetupAttachment(RootComponent);
    DetectionRangeWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    DetectionRangeWidgetComp->SetDrawSize(FVector2D(DetectionRange * 2.0f, DetectionRange * 2.0f));
    DetectionRangeWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // 캐릭터 발 아래에 위치
    DetectionRangeWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 수면 상태 UI 위젯 컴포넌트 생성
    SleepStateWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("SleepStateWidget"));
    SleepStateWidgetComp->SetupAttachment(RootComponent);
    SleepStateWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    SleepStateWidgetComp->SetDrawSize(FVector2D(100.0f, 100.0f));
    SleepStateWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f)); // 캐릭터 머리 위에 위치
    SleepStateWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SleepStateWidgetComp->SetVisibility(false); // 기본적으로 비활성화

    // Tick 활성화
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    
    if (this->IsA(ABossCharacter::StaticClass()))
    {
        DetectionRange = 2500.0f;
        
        // 감지 범위 UI 위젯 크기 조정
        if (DetectionRangeWidgetComp)
        {
            DetectionRangeWidgetComp->SetDrawSize(FVector2D(DetectionRange * 2.0f, DetectionRange * 2.0f));
        }
    }
    
    UpdateMovementSpeed(); // 초기 속도 설정

    // 무기 애니메이션 인스턴스 확인
    if (WeaponMesh)
    {
        UAnimInstance *WeaponAnimInstance = WeaponMesh->GetAnimInstance();
        if (WeaponAnimInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Instance is valid"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Instance is NULL - 에디터에서 무기 메시에 애니메이션 블루프린트를 설정해주세요"));
        }
    }

    // 걷기 애니메이션 시작
    if (WeaponWalkMontage)
    {
        PlayWeaponAnimation(WeaponWalkMontage);
    }

    // 위젯과 함수 포인터 캐싱
    if (DetectionRangeWidgetComp)
    {
        CachedDetectionWidget = Cast<UUserWidget>(DetectionRangeWidgetComp->GetWidget());
        if (CachedDetectionWidget)
        {
            CachedUpdateFunc = CachedDetectionWidget->FindFunction(FName("UpdateDetectionState"));
        }
    }

    // 수면 상태 위젯 초기화
    if (SleepStateWidgetComp)
    {
        // 위젯을 World 스페이스로 유지하면서 빌보드 효과 설정
        SleepStateWidgetComp->SetWidgetSpace(EWidgetSpace::World);

        // 위젯 렌더링 설정 조정
        SleepStateWidgetComp->SetDrawAtDesiredSize(true);
        SleepStateWidgetComp->SetPivot(FVector2D(0.5f, 0.5f));

        if (UUserWidget *Widget = Cast<UUserWidget>(SleepStateWidgetComp->GetWidget()))
        {
            // 위젯에 Enemy 참조 설정
            if (UFunction *InitFunc = Widget->FindFunction(FName("InitializeEnemy")))
            {
                struct
                {
                    AEnemyCharacter *Enemy;
                } Params;
                Params.Enemy = this;
                Widget->ProcessEvent(InitFunc, &Params);

                UE_LOG(LogTemp, Warning, TEXT("수면 상태 위젯 초기화 - Enemy 참조 설정"));
            }
        }
    }
}

// Tick 함수 구현
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 수면 위젯이 항상 카메라를 향하도록 회전
    if (SleepStateWidgetComp && SleepStateWidgetComp->IsVisible())
    {
        // 플레이어 카메라 위치 가져오기
        APlayerController *PC = GetWorld()->GetFirstPlayerController();
        if (PC && PC->PlayerCameraManager)
        {
            FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
            FVector Direction = CameraLocation - SleepStateWidgetComp->GetComponentLocation();
            Direction.Z = 0.0f; // 수평 방향만 고려 (선택적)

            if (!Direction.IsNearlyZero())
            {
                FRotator NewRotation = Direction.Rotation();
                SleepStateWidgetComp->SetWorldRotation(NewRotation);
            }
        }
    }

    // 수면 상태일 때 타이머 업데이트
    if (bIsSleeping && SleepDuration > 0.0f)
    {
        SleepRemainingTime = FMath::Max(0.0f, SleepRemainingTime - DeltaTime);

        // 수면 UI 업데이트
        if (SleepStateWidgetComp && SleepStateWidgetComp->IsVisible())
        {
            if (UUserWidget *Widget = Cast<UUserWidget>(SleepStateWidgetComp->GetWidget()))
            {
                if (UFunction *UpdateFunc = Widget->FindFunction(FName("UpdateSleepUI")))
                {
                    struct
                    {
                        float RemainingTime;
                    } Params;

                    Params.RemainingTime = SleepRemainingTime;
                    Widget->ProcessEvent(UpdateFunc, &Params);

                    UE_LOG(LogTemp, Warning, TEXT("수면 UI 업데이트: %.1f초 남음"), SleepRemainingTime);
                }
            }
        }

        // 수면 시간이 끝나면 깨우기
        if (SleepRemainingTime <= 0.0f)
        {
            WakeUp();
        }
    }
}

// 플레이어 감지 상태 설정
void AEnemyCharacter::SetPlayerDetected(bool bDetected)
{
    if (bPlayerDetected != bDetected)
    {
        bPlayerDetected = bDetected;

        if (CachedDetectionWidget && CachedUpdateFunc)
        {
            struct
            {
                bool bIsDetected;
            } Params;

            Params.bIsDetected = bDetected;
            CachedDetectionWidget->ProcessEvent(CachedUpdateFunc, &Params);
        }

        // 블루프린트에서 UI 업데이트를 위한 로그
        UE_LOG(LogTemp, Warning, TEXT("플레이어 감지 상태 변경: %s"), bPlayerDetected ? TEXT("감지됨") : TEXT("감지되지 않음"));
    }
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                  class AController *EventInstigator, AActor *DamageCauser)
{
    // 사망 상태 확인
    if (bIsDead)
        return 0.0f;

    // 기본 TakeDamage 호출
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 데미지 원인 로깅
    if (DamageCauser)
    {
        UE_LOG(LogTemp, Warning, TEXT("데미지 원인: %s, 데미지 양: %f"), *DamageCauser->GetName(), ActualDamage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("알 수 없는 원인의 데미지: %f"), ActualDamage);
    }

    // 인터페이스 구현 호출 (실제 데미지 처리)
    TakeDamage(ActualDamage);

    return ActualDamage;
}

// 인터페이스 구현
void AEnemyCharacter::TakeDamage(float DamageAmount)
{
    if (bIsDead)
        return;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    UE_LOG(LogTemp, Warning, TEXT("Enemy took damage: %f, Current Health: %f"), DamageAmount, CurrentHealth);

    // 현재 공격 애니메이션이 재생 중이면 중단
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && AttackMontage && AnimInstance->Montage_IsPlaying(AttackMontage))
    {
        AnimInstance->Montage_Stop(0.1f, AttackMontage);
        UE_LOG(LogTemp, Warning, TEXT("Stopping attack animation due to hit"));

        // 공격 타이머 취소
        GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

        // 공격 쿨다운 재설정 (피격 후 바로 공격하지 못하도록)
        bCanAttack = false;
        GetWorld()->GetTimerManager().SetTimer(
            AttackCooldownTimer,
            [this]()
            { bCanAttack = true; },
            AttackCooldown * 0.5f, // 피격 후에는 쿨다운을 절반으로 줄임
            false);
    }

    // 피격 애니메이션 재생
    if (HitReactionMontage)
    {
        // AI 컨트롤러의 행동 일시 중지
        if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
        {
            AIController->GetBrainComponent()->StopLogic("Hit Reaction");
            UE_LOG(LogTemp, Warning, TEXT("Stopping AI behavior for hit reaction"));

            // 이동 중지
            GetCharacterMovement()->StopMovementImmediately();
        }

        // 현재 재생 중인 애니메이션을 중지하고 새로운 피격 애니메이션 재생
        StopAnimMontage();

        // 피격 애니메이션 재생 속도 설정
        float PlayRate = 1.5f;

        // 원래 애니메이션 길이를 가져옴
        float OriginalDuration = PlayAnimMontage(HitReactionMontage, PlayRate);

        // PlayRate를 고려한 실제 재생 시간 계산
        float ActualDuration = OriginalDuration / PlayRate;

        UE_LOG(LogTemp, Warning, TEXT("Playing hit reaction animation - Original Duration: %f, Actual Duration: %f"),
               OriginalDuration, ActualDuration);

        // 피격 애니메이션이 끝난 후 AI 행동 재개를 위한 타이머 설정
        FTimerHandle HitRecoveryTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            HitRecoveryTimerHandle,
            [this]()
            {
                // AI 컨트롤러의 행동 재개
                if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
                {
                    // 행동 로직 재개
                    AIController->GetBrainComponent()->StartLogic();

                    // 현재 행동 트리 재시작
                    AIController->GetBrainComponent()->RestartLogic();

                    UE_LOG(LogTemp, Warning, TEXT("Hit reaction finished, resuming AI behavior"));
                }
            },
            ActualDuration * 0.9f, // 애니메이션이 거의 끝날 때 행동 재개
            false);
    }

    // 피격 사운드 재생
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
        UE_LOG(LogTemp, Warning, TEXT("Playing hit sound"));
    }

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void AEnemyCharacter::Die()
{
    if (bIsDead)
        return;
    bIsDead = true;

    // 킬 카운트 증가
    if (UGameInstance *GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
    {
        if (UBasicGameInstance *BasicGameInstance = Cast<UBasicGameInstance>(GameInstance))
        {
            BasicGameInstance->AddKill(); // 킬 카운트만 증가
            BasicGameInstance->AddScore(300);
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
    if (AController *AIController = GetController())
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
    if (bIsDead || !bCanAttack)
        return;

    // 현재 피격 애니메이션이 재생 중인지 확인
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HitReactionMontage && AnimInstance->Montage_IsPlaying(HitReactionMontage))
    {
        // 피격 애니메이션이 재생 중이면 공격하지 않음
        UE_LOG(LogTemp, Warning, TEXT("Cannot attack while hit reaction animation is playing"));
        return;
    }

    // 플레이어 방향으로 회전
    AActor *Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
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
            [this, Player]()
            {
                // 플레이어가 유효하고 살아있는지 확인
                AFPSCharacter *FPSPlayer = Cast<AFPSCharacter>(Player);
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
            ActualDuration * 0.3f, // 애니메이션의 50% 지점에서 데미지 계산
            false);

        // 실제 재생 시간으로 타이머 설정
        GetWorld()->GetTimerManager().SetTimer(
            AttackTimerHandle,
            [this]()
            {
                // AI 컨트롤러의 행동 재개
                if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
                {
                    // 행동 로직 재개
                    AIController->GetBrainComponent()->StartLogic();

                    // 현재 행동 트리 재시작
                    AIController->GetBrainComponent()->RestartLogic();

                    // 플레이어를 향해 즉시 이동 명령
                    if (AActor *Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
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
            false);

        // AI 컨트롤러의 행동 중지
        if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
        {
            AIController->GetBrainComponent()->StopLogic("Attacking");
        }
    }

    // 공격 쿨다운 설정
    bCanAttack = false;
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        [this]()
        { bCanAttack = true; },
        AttackCooldown,
        false);

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

void AEnemyCharacter::MoveTo(FVector TargetLocation)
{
    AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController());
    if (bIsDead || (AIController && AIController->IsInAttackRange()))
        return;

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

void AEnemyCharacter::PlayAnimation(UAnimMontage *Animation)
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
    if (bIsDead || bIsSleeping)
        return;

    bIsSleeping = true;
    SleepDuration = Duration;
    SleepRemainingTime = Duration;

    // 수면 UI 활성화
    if (SleepStateWidgetComp)
    {
        SleepStateWidgetComp->SetVisibility(true);
        UE_LOG(LogTemp, Warning, TEXT("수면 UI 활성화, 지속 시간: %.1f초"), Duration);
    }

    // AI 이동 중지
    GetCharacterMovement()->StopMovementImmediately();

    // AI 컨트롤러 비활성화
    if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StopLogic("Sleeping");
    }

    // 게임 인스턴스에 수면 카운트 증가
    if (UBasicGameInstance *GameInstance = Cast<UBasicGameInstance>(GetWorld()->GetGameInstance()))
    {
        GameInstance->AddSleep();
    }

    // 지정된 시간 후에 깨어나기
    GetWorld()->GetTimerManager().SetTimer(
        SleepTimerHandle,
        this,
        &AEnemyCharacter::WakeUp,
        Duration,
        false);

    UE_LOG(LogTemp, Warning, TEXT("Enemy is now sleeping for %f seconds"), Duration);
}

void AEnemyCharacter::WakeUp()
{
    if (bIsDead)
        return;

    bIsSleeping = false;
    SleepRemainingTime = 0.0f;

    // 수면 UI 비활성화
    if (SleepStateWidgetComp)
    {
        SleepStateWidgetComp->SetVisibility(false);
        UE_LOG(LogTemp, Warning, TEXT("수면 UI 비활성화"));
    }

    // AI 컨트롤러 다시 활성화
    if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StartLogic();
    }

    UE_LOG(LogTemp, Warning, TEXT("Enemy woke up from sleep"));
}

void AEnemyCharacter::UpdateDetectionRangeForPlayerState(AFPSCharacter *Player)
{
    if (!Player)
        return;
        
    // 보스 캐릭터인지 확인
    if (this->IsA(ABossCharacter::StaticClass()))
    {
        float OldDetectionRange = DetectionRange;
        DetectionRange = 2500.0f;
        
        // 감지 범위가 변경되었으면 UI 위젯 크기도 업데이트
        if (OldDetectionRange != DetectionRange && DetectionRangeWidgetComp)
        {
            DetectionRangeWidgetComp->SetDrawSize(FVector2D(DetectionRange * 2.0f, DetectionRange * 2.0f));
        }
        return;
    }

    ECharacterState CurrentPlayerState = Player->GetCurrentState();
    float OldDetectionRange = DetectionRange;

    switch (CurrentPlayerState)
    {
    case ECharacterState::Sprinting:
        DetectionRange = BaseDetectionRange * SprintingRangeMultiplier; // 달리기: 기본 범위의 1.5배
        break;

    case ECharacterState::Crouching:
        DetectionRange = BaseDetectionRange * CrouchingRangeMultiplier; // 앉기: 기본 범위의 0.5배
        break;

    case ECharacterState::Dead:
        DetectionRange = 0.0f; // 사망: 감지 안함
        break;

    default:                     // Normal 상태
        DetectionRange = BaseDetectionRange; // 걷기: 기본 감지 범위
        break;
    }

    // 감지 범위가 변경되었으면 UI 위젯 크기도 업데이트
    if (OldDetectionRange != DetectionRange && DetectionRangeWidgetComp)
    {
        DetectionRangeWidgetComp->SetDrawSize(FVector2D(DetectionRange * 2.0f, DetectionRange * 2.0f));
    }
}

// 무기 애니메이션 재생 함수 추가
void AEnemyCharacter::PlayWeaponAnimation(UAnimMontage *WeaponAnimation)
{
    if (WeaponAnimation && WeaponMesh)
    {
        // 무기 메시의 애니메이션 인스턴스 가져오기
        UAnimInstance *WeaponAnimInstance = WeaponMesh->GetAnimInstance();
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

// PlayRate 매개변수를 포함한 무기 애니메이션 재생 함수 오버로드
void AEnemyCharacter::PlayWeaponAnimation(UAnimMontage *WeaponAnimation, float PlayRate)
{
    if (WeaponAnimation && WeaponMesh)
    {
        // 무기 메시의 애니메이션 인스턴스 가져오기
        UAnimInstance *WeaponAnimInstance = WeaponMesh->GetAnimInstance();
        if (WeaponAnimInstance)
        {
            float Duration = WeaponAnimInstance->Montage_Play(WeaponAnimation, PlayRate);
            UE_LOG(LogTemp, Warning, TEXT("Weapon Animation Duration: %f, PlayRate: %f"), Duration, PlayRate);
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
