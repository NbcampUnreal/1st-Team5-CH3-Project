#include "BossCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.h"
#include "FPSCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BasicGameState.h"
#include "BasicGameInstance.h"

ABossCharacter::ABossCharacter()
{
    // 기본 값 설정
    AttackSpeedMultiplier = 2.0f;    // 일반 Enemy보다 2배 빠른 공격 속도
    MovementSpeedMultiplier = 2.0f;  // 일반 Enemy보다 2배 빠른 이동 속도
    
    DetectionRange = 2500.0f;

    AttackRange = 350.0f; // 기존 값보다 크게 설정
    
    MaxHealth = 1200.0f;

    // 체력 UI 초기화
    bHealthUIVisible = false;
    HealthUIWidget = nullptr;
}

void ABossCharacter::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();

    // 현재 체력 설정
    CurrentHealth = MaxHealth;

    // 이동 속도 조정 - UpdateMovementSpeed 함수 호출로 대체
    UpdateMovementSpeed();

    // 감지 범위와 공격 범위 명시적으로 설정
    DetectionRange = 2500.0f;
    AttackRange = 350.0f;
    
    // 감지 범위 UI 위젯 크기 조정
    if (DetectionRangeWidgetComp)
    {
        DetectionRangeWidgetComp->SetDrawSize(FVector2D(DetectionRange * 2.0f, DetectionRange * 2.0f));
    }

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    UE_LOG(LogTemp, Warning, TEXT("보스 캐릭터 감지 범위 설정: %f (시야각 제한 없음)"), DetectionRange);
#endif

    // 충돌 컴포넌트 조정 - 보스의 크기에 맞게 조정
    UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
    if (CapsuleComp)
    {
        float OriginalRadius = CapsuleComp->GetUnscaledCapsuleRadius();
        float OriginalHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();

        // 캡슐 크기를 공격 범위에 맞게 조정
        CapsuleComp->SetCapsuleSize(OriginalRadius * 1.2f, OriginalHalfHeight * 1.5f);

        UE_LOG(LogTemp, Warning, TEXT("보스 충돌 캡슐 크기 조정: 반경 %f, 높이 %f"),
               CapsuleComp->GetUnscaledCapsuleRadius(),
               CapsuleComp->GetUnscaledCapsuleHalfHeight() * 2.0f);
    }

    // 감지 범위 UI 위젯 활성화 및 크기 조정
    if (DetectionRangeWidgetComp)
    {
        DetectionRangeWidgetComp->SetVisibility(true);

        // 위젯 크기를 감지 범위에 맞게 조정
        UUserWidget *Widget = DetectionRangeWidgetComp->GetUserWidgetObject();
        if (Widget)
        {
            // 위젯에 감지 범위 값 전달
            UFunction *UpdateRangeFunc = Widget->FindFunction(FName("UpdateDetectionRange"));
            if (UpdateRangeFunc)
            {
                struct
                {
                    float Range;
                } Params;
                Params.Range = DetectionRange;
                Widget->ProcessEvent(UpdateRangeFunc, &Params);
            }
        }
    }

#if UE_BUILD_DEBUG
    UE_LOG(LogTemp, Warning, TEXT("보스 캐릭터가 생성되었습니다. 체력: %f, 감지 범위: %f, 공격 범위: %f"),
           CurrentHealth, DetectionRange, AttackRange);
#endif
}

void ABossCharacter::Tick(float DeltaTime)
{
    // 부모 클래스의 Tick 함수 호출
    Super::Tick(DeltaTime);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    // 디버그 정보 출력 (3초마다)
    static float LastDebugTime = 0.0f;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastDebugTime > 3.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("보스 상태 - 체력: %f/%f, 수면: %s"), 
            CurrentHealth, MaxHealth, bIsSleeping ? TEXT("O") : TEXT("X"));
        LastDebugTime = CurrentTime;
    }
#endif

    // 수면 상태일 때 남은 시간 업데이트
    if (bIsSleeping)
    {
        SleepRemainingTime = FMath::Max(0.0f, SleepRemainingTime - DeltaTime);
    }
}

// 공격 검증 함수 - 보스의 공격 범위 검사 로직 개선
bool ABossCharacter::IsInAttackRange(AActor *Target) const
{
    if (!Target)
    {
        return false;
    }

    // 기본 거리 계산
    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());

    FVector DirectionToTarget = Target->GetActorLocation() - GetActorLocation();
    DirectionToTarget.Z = 0; // 높이 차이 무시
    float HorizontalDistance = DirectionToTarget.Size();

    // 보스의 앞에 있는지 확인 (전방 180도 내에 있으면 공격 가능)
    FVector ForwardVector = GetActorForwardVector();
    ForwardVector.Z = 0; // 높이 차이 무시
    ForwardVector.Normalize();

    float DotProduct = FVector::DotProduct(ForwardVector, DirectionToTarget.GetSafeNormal());
    bool bIsInFront = DotProduct > -0.5f; // 전방 180도 내에 있으면 true

    // 공격 범위 내에 있고 전방에 있으면 공격 가능
    bool bInRange = HorizontalDistance <= AttackRange * 1.2f; // 약간의 여유 제공

    if (bInRange && bIsInFront)
    {
#if UE_BUILD_DEBUG
        UE_LOG(LogTemp, Warning, TEXT("보스 공격 범위 내 타겟 감지: 거리 %f, 공격범위 %f"), HorizontalDistance, AttackRange);
#endif
    }

    return bInRange && bIsInFront;
}

float ABossCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                 class AController *EventInstigator, AActor *DamageCauser)
{
    // 보스는 데미지를 약간 덜 받도록 설정 (방어력 증가)
    float ModifiedDamage = DamageAmount * 0.8f;

    // 체력 UI 표시 (데미지를 입은 순간에만)
    if (!bHealthUIVisible)
    {
        ShowBossHealthUI();
    }

    // 부모 클래스의 TakeDamage 호출
    return Super::TakeDamage(ModifiedDamage, DamageEvent, EventInstigator, DamageCauser);
}

// 인터페이스 구현 - 부모 클래스의 TakeDamage 함수 오버라이드
void ABossCharacter::TakeDamage(float DamageAmount)
{
    if (bIsDead)
        return;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    UE_LOG(LogTemp, Warning, TEXT("Boss took damage: %f, Current Health: %f"), DamageAmount, CurrentHealth);

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

        // 피격 애니메이션 재생 속도 설정 - 보스는 더 빠르게 (5.0)
        float PlayRate = 5.0f;

        // 원래 애니메이션 길이를 가져옴
        float OriginalDuration = PlayAnimMontage(HitReactionMontage, PlayRate);

        // PlayRate를 고려한 실제 재생 시간 계산
        float ActualDuration = OriginalDuration / PlayRate;

        UE_LOG(LogTemp, Warning, TEXT("Boss playing hit reaction animation - Original Duration: %f, Actual Duration: %f"),
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

                    UE_LOG(LogTemp, Warning, TEXT("Boss hit reaction finished, resuming AI behavior"));
                }
            },
            ActualDuration * 0.9f, // 애니메이션이 거의 끝날 때 행동 재개
            false);
    }

    // 피격 사운드 재생
    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
        UE_LOG(LogTemp, Warning, TEXT("Playing boss hit sound"));
    }

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

// 일반 공격 함수 구현
void ABossCharacter::NormalAttack()
{
    if (!IsAlive() || !bCanAttack)
        return;

    // 현재 피격 애니메이션이 재생 중인지 확인
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HitReactionMontage && AnimInstance->Montage_IsPlaying(HitReactionMontage))
    {
        // 피격 애니메이션이 재생 중이면 공격하지 않음
        UE_LOG(LogTemp, Warning, TEXT("보스가 피격 애니메이션 중이라 공격할 수 없습니다"));
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

    // 공격 애니메이션 재생 - 보스 전용 공격 속도 배율 적용
    if (AttackMontage)
    {
        // 기본 PlayRate에 보스 공격 속도 배율 적용
        float PlayRate = 2.5f * AttackSpeedMultiplier;
        
        UE_LOG(LogTemp, Warning, TEXT("보스 일반 공격 시작! 공격 속도 배율: %f, 최종 PlayRate: %f"), 
               AttackSpeedMultiplier, PlayRate);
        
        // 원래 애니메이션 길이를 가져옴
        float OriginalDuration = PlayAnimMontage(AttackMontage, PlayRate);

        // 무기 공격 애니메이션도 재생 - 보스 공격 속도 배율 적용
        if (WeaponAttackMontage)
        {
            // 무기 애니메이션에도 동일한 PlayRate 적용
            PlayWeaponAnimation(WeaponAttackMontage, PlayRate);
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
#if UE_BUILD_DEBUG
                        UE_LOG(LogTemp, Warning, TEXT("보스 일반 공격이 플레이어에게 적중! 데미지: %f"), WeaponDamage);
#endif
                        FPSPlayer->TakeDamage(WeaponDamage);
                    }
                    else
                    {
#if UE_BUILD_DEBUG
                        UE_LOG(LogTemp, Warning, TEXT("보스 일반 공격 실패! 거리: %f, 각도: %f"),
                               DistToPlayer, FMath::Acos(DotProduct) * 180.0f / PI);
#endif
                    }
                }
            },
            ActualDuration * 0.5f, // 애니메이션의 50% 지점에서 데미지 계산
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
                                PlayWeaponAnimation(WeaponWalkMontage, 1.0f);
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

    // 공격 쿨다운 설정 - 보스는 더 짧은 쿨다운 적용
    bCanAttack = false;
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        [this]()
        { bCanAttack = true; },
        AttackCooldown / AttackSpeedMultiplier, // 공격 속도 배율로 쿨다운 감소
        false);

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

// 보스 공격 함수 오버라이드 - 1/2 확률로 특수 공격 발동
void ABossCharacter::Attack()
{
    // 랜덤하게 1/2 확률로 특수 공격 또는 일반 공격 선택
    bool bUseSpecialAttack = (FMath::RandRange(0, 1) == 0);
    
    if (bUseSpecialAttack)
    {
#if UE_BUILD_DEBUG
        UE_LOG(LogTemp, Warning, TEXT("보스가 특수 공격을 시도합니다!"));
#endif
        SpecialAttack();
    }
    else
    {
#if UE_BUILD_DEBUG
        UE_LOG(LogTemp, Warning, TEXT("보스가 일반 공격을 시도합니다."));
#endif
        NormalAttack();
    }
}

void ABossCharacter::SpecialAttack()
{
    if (!IsAlive() || !bCanAttack)
        return;

    // 현재 피격 애니메이션이 재생 중인지 확인
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HitReactionMontage && AnimInstance->Montage_IsPlaying(HitReactionMontage))
    {
        // 피격 애니메이션이 재생 중이면 공격하지 않음
#if UE_BUILD_DEBUG
        UE_LOG(LogTemp, Warning, TEXT("보스가 피격 애니메이션 중이라 특수 공격할 수 없습니다"));
#endif
        return;
    }

    // 플레이어 방향으로 회전
    AActor *Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player)
        return;

    FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    FRotator NewRotation = Direction.Rotation();
    SetActorRotation(NewRotation);

    // 이동 중지
    GetCharacterMovement()->StopMovementImmediately();

    // AI 컨트롤러의 행동 중지
    if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StopLogic("SpecialAttacking");
    }

    // 10번 연속 공격 실행
    int32 TotalAttacks = 10;
    float AttackInterval = 0.2f; // 0.2초 간격으로 공격
    float SpecialPlayRate = AttackSpeedMultiplier * 5.0f; // 5배 빠른 속도
    float SpecialDamage = WeaponDamage * 0.1f; // 데미지는 1/10

    // 첫 번째 공격 즉시 실행
    ExecuteSpecialAttack(Player, 1, TotalAttacks, SpecialPlayRate, SpecialDamage);

    // 나머지 9번의 공격을 타이머로 예약
    for (int32 i = 2; i <= TotalAttacks; i++)
    {
        FTimerHandle SpecialAttackTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            SpecialAttackTimerHandle,
            [this, Player, i, TotalAttacks, SpecialPlayRate, SpecialDamage]()
            {
                ExecuteSpecialAttack(Player, i, TotalAttacks, SpecialPlayRate, SpecialDamage);
            },
            AttackInterval * (i - 1), // 각 공격마다 시간 간격 증가
            false);
    }

    // 모든 공격이 끝난 후 AI 컨트롤러 행동 재개
    FTimerHandle FinishTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        FinishTimerHandle,
        [this]()
        {
            // AI 컨트롤러의 행동 재개
            if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
            {
                // 행동 로직 재개
                AIController->GetBrainComponent()->StartLogic();
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
                            PlayWeaponAnimation(WeaponWalkMontage, 1.0f);
                        }
                    }
                }
            }

            // 공격 쿨다운 설정 - 특수 공격 후 더 긴 쿨다운 적용
            bCanAttack = false;
            GetWorld()->GetTimerManager().SetTimer(
                AttackCooldownTimer,
                [this]() { bCanAttack = true; },
                AttackCooldown * 2.0f, // 특수 공격 후 쿨다운은 2배로 설정
                false);
        },
        AttackInterval * TotalAttacks + 0.5f,
        false);
}

// 특수 공격의 각 단계를 실행하는 헬퍼 함수
void ABossCharacter::ExecuteSpecialAttack(AActor* Player, int32 AttackIndex, int32 TotalAttacks, float SpecialPlayRate, float SpecialDamage)
{
    if (!IsAlive() || !Player)
        return;

    // 공격 애니메이션 재생
    if (AttackMontage)
    {
        // 애니메이션 재생
        PlayAnimMontage(AttackMontage, SpecialPlayRate);
        
#if UE_BUILD_DEBUG
        UE_LOG(LogTemp, Warning, TEXT("보스 특수 공격 %d/%d 애니메이션 재생 - 속도: %f"), 
               AttackIndex, TotalAttacks, SpecialPlayRate);
#endif

        // 무기 공격 애니메이션도 재생
        if (WeaponAttackMontage)
        {
            PlayWeaponAnimation(WeaponAttackMontage, SpecialPlayRate);
        }
    }

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
        if (DistToPlayer <= AttackRange * 1.5f && DotProduct >= 0.5f) // 특수 공격은 더 넓은 범위와 각도
        {
            UE_LOG(LogTemp, Warning, TEXT("보스 특수 공격 %d/%d 적중! 데미지: %f"), 
                   AttackIndex, TotalAttacks, SpecialDamage);
            FPSPlayer->TakeDamage(SpecialDamage);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("보스 특수 공격 %d/%d 실패! 거리: %f, 각도: %f"),
                   AttackIndex, TotalAttacks, DistToPlayer, FMath::Acos(DotProduct) * 180.0f / PI);
        }
    }

    // 공격 효과음 재생
    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation(), 
                                             FMath::RandRange(0.8f, 1.2f)); // 약간 다른 피치로 재생
    }
}

// 보스 전용 이동 속도 업데이트 함수
void ABossCharacter::UpdateMovementSpeed()
{
    // 부모 클래스의 기본 속도 계산
    float BaseSpeed = bIsChasing ? ChaseSpeed : PatrolSpeed;
    
    // 보스 전용 속도 배율 적용
    float BossSpeed = BaseSpeed * MovementSpeedMultiplier;
    
    // 이동 속도 설정
    GetCharacterMovement()->MaxWalkSpeed = BossSpeed;
    
    // 로그 메시지 제거 (디버그 빌드에서만 출력)
#if UE_BUILD_DEBUG
    UE_LOG(LogTemp, Warning, TEXT("보스 이동 속도 업데이트: %f (기본 속도: %f, 배율: %f)"), 
           BossSpeed, BaseSpeed, MovementSpeedMultiplier);
#endif
}

// 무기 애니메이션 재생 함수 오버로드 구현
void ABossCharacter::PlayWeaponAnimation(UAnimMontage* WeaponAnimation, float PlayRate)
{
    if (WeaponAnimation && WeaponMesh)
    {
        UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
        if (WeaponAnimInstance)
        {
            WeaponAnimInstance->Montage_Play(WeaponAnimation, PlayRate);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("보스 무기 애니메이션 또는 WeaponMesh가 null입니다!"));
    }
}

// 보스 체력 UI 표시 함수
void ABossCharacter::ShowBossHealthUI()
{
    // 이미 표시 중이면 무시
    if (bHealthUIVisible)
    {
        return;
    }
    
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }
    
    // 체력 UI 위젯 생성
    if (HealthUIWidgetClass)
    {
        HealthUIWidget = CreateWidget<UUserWidget>(PC, HealthUIWidgetClass);
        if (HealthUIWidget)
        {
            HealthUIWidget->AddToViewport();
            bHealthUIVisible = true;
            
            UE_LOG(LogTemp, Warning, TEXT("보스 체력 UI가 표시되었습니다."));
        }
    }
}

// 보스 체력 UI 숨김 함수
void ABossCharacter::HideBossHealthUI()
{
    // 이미 숨겨져 있으면 무시
    if (!bHealthUIVisible || !HealthUIWidget)
    {
        return;
    }
    
    // 위젯 제거
    HealthUIWidget->RemoveFromParent();
    HealthUIWidget = nullptr;
    bHealthUIVisible = false;
    
    UE_LOG(LogTemp, Warning, TEXT("보스 체력 UI가 숨겨졌습니다."));
}

void ABossCharacter::Sleep(float Duration)
{
    if (bIsDead || bIsSleeping)
        return;

    // 수면 상태 설정
    bIsSleeping = true;
    SleepDuration = 1.0f; // 항상 1.5초로 고정
    SleepRemainingTime = 1.0f;

    // 수면 UI 활성화
    if (SleepStateWidgetComp)
    {
        SleepStateWidgetComp->SetVisibility(true);
    }

    // 진행 중인 공격 애니메이션 중지
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        // 공격 애니메이션 중지
        if (AttackMontage && AnimInstance->Montage_IsPlaying(AttackMontage))
        {
            AnimInstance->Montage_Stop(0.1f, AttackMontage);
        }
        
        // 무기 애니메이션 중지
        if (WeaponAttackMontage && WeaponMesh)
        {
            UAnimInstance *WeaponAnimInstance = WeaponMesh->GetAnimInstance();
            if (WeaponAnimInstance && WeaponAnimInstance->Montage_IsPlaying(WeaponAttackMontage))
            {
                WeaponAnimInstance->Montage_Stop(0.1f, WeaponAttackMontage);
            }
        }
    }

    // 공격 관련 타이머 초기화
    GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(AttackCooldownTimer);
    bCanAttack = false; // 수면 상태에서는 공격 불가

    // AI 이동 중지
    GetCharacterMovement()->StopMovementImmediately();

    // AI 컨트롤러 비활성화
    if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StopLogic("Sleeping");
    }

    // 게임 인스턴스에 수면 카운트 증가
    if (UBasicGameInstance *GameInstance = Cast<UBasicGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        GameInstance->AddSleep();
        GameInstance->AddScore(500);
    }

    // 기존 타이머 제거 후 새 타이머 설정 (항상 1초)
    GetWorld()->GetTimerManager().ClearTimer(SleepTimerHandle);
    
    // 타이머 델리게이트 생성
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUObject(this, &ABossCharacter::WakeUp);
    
    // 타이머 설정 (항상 1초)
    GetWorld()->GetTimerManager().SetTimer(
        SleepTimerHandle,
        TimerDelegate,
        1.0f, // 항상 1초로 고정
        false);
    
    UE_LOG(LogTemp, Warning, TEXT("보스가 수면 상태에 들어갔습니다. 수면 시간: 1초"));
}

void ABossCharacter::WakeUp()
{
    UE_LOG(LogTemp, Warning, TEXT("보스가 깨어납니다."));
    if (bIsDead)
        return;

    bIsSleeping = false;
    SleepRemainingTime = 0.0f;

    // 수면 UI 비활성화
    if (SleepStateWidgetComp)
    {
        SleepStateWidgetComp->SetVisibility(false);
        UE_LOG(LogTemp, Warning, TEXT("보스 수면 UI 비활성화"));
    }

    // 공격 가능 상태 복원 (일정 시간 후)
    GetWorld()->GetTimerManager().SetTimer(
        AttackCooldownTimer,
        [this]()
        { bCanAttack = true; },
        AttackCooldown * 0.5f, // 깨어난 후 짧은 쿨다운
        false);

    // AI 컨트롤러 재활성화
    if (AEnemyAIController *AIController = Cast<AEnemyAIController>(GetController()))
    {
        AIController->GetBrainComponent()->StartLogic();
        UE_LOG(LogTemp, Warning, TEXT("보스 AI 컨트롤러 재활성화"));
    }
}

// 보스 체력 비율 반환 함수
float ABossCharacter::GetHealthPercent() const
{
    return CurrentHealth / MaxHealth;
}

// Die 함수 수정 (함수 이름은 실제 코드에 맞게 조정)
void ABossCharacter::Die()
{
    // 체력 UI 숨기기
    HideBossHealthUI();
    
    // 기존 Die 함수 내용
    Super::Die();
}