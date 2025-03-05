#include "BossAIController.h"
#include "BossCharacter.h"
#include "FPSCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"

ABossAIController::ABossAIController()
{
}

void ABossAIController::BeginPlay()
{
    Super::BeginPlay();

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    UE_LOG(LogTemp, Warning, TEXT("보스 AI 컨트롤러가 시작되었습니다."));
#endif
}

void ABossAIController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    ABossCharacter *BossCharacter = Cast<ABossCharacter>(InPawn);
    if (BossCharacter)
    {
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
        UE_LOG(LogTemp, Warning, TEXT("보스 AI 컨트롤러가 보스 캐릭터를 제어합니다."));
#endif

        if (BossBehaviorTree)
        {
            BlackboardComponent->InitializeBlackboard(*BossBehaviorTree->BlackboardAsset);
            BehaviorTreeComponent->StartTree(*BossBehaviorTree);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
            UE_LOG(LogTemp, Warning, TEXT("보스 전용 행동 트리를 시작합니다."));
#endif
        }
    }
}

void ABossAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 보스 전용 플레이어 감지 상태 업데이트
    UpdateBossPlayerDetection();
}

bool ABossAIController::CanSeePlayerBoss()
{
    // 부모 클래스의 CanSeePlayer 함수 대신 보스 전용 감지 로직 구현
    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ABossCharacter *Boss = Cast<ABossCharacter>(GetPawn());

    if (!PlayerPawn || !Boss)
    {
        return false;
    }

    // 거리 계산
    float Distance = FVector::Dist(Boss->GetActorLocation(), PlayerPawn->GetActorLocation());
    
    float DetectionRange = 2000.0f;
    
    // 감지 범위 내에 있는지 확인
    if (Distance <= DetectionRange)
    {
        // 시야선 체크 (벽이나 장애물 체크)
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Boss);
        
        FVector StartLocation = Boss->GetActorLocation();
        StartLocation.Z += 50.0f;  // 캐릭터 중심부에서 체크하도록 높이 조정
        
        FVector EndLocation = PlayerPawn->GetActorLocation();
        EndLocation.Z += 50.0f;    // 플레이어 중심부 체크
        
        // 시야선이 막혀있는지 확인
        bool bHasLineOfSight = !GetWorld()->LineTraceSingleByChannel(
            HitResult, 
            StartLocation, 
            EndLocation, 
            ECC_Visibility, 
            QueryParams
        );
        
        if (bHasLineOfSight)
        {
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
            static float LastLogTime = 0.0f;
            float CurrentTime = GetWorld()->GetTimeSeconds();
            if (CurrentTime - LastLogTime > 3.0f)
            {
                UE_LOG(LogTemp, Warning, TEXT("보스가 플레이어를 감지했습니다! 거리: %f"), Distance);
                LastLogTime = CurrentTime;
            }
#endif
            return true;
        }
    }
    
    return false;
}

bool ABossAIController::IsInAttackRange()
{
    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ABossCharacter *Boss = Cast<ABossCharacter>(GetPawn());

    if (!PlayerPawn || !Boss)
    {
        return false;
    }

    // 보스 캐릭터의 공격 범위 체크 함수 사용
    bool bInRange = Boss->IsInAttackRange(PlayerPawn);

    if (bInRange)
    {
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
        static float LastAttackLogTime = 0.0f;
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - LastAttackLogTime > 3.0f) 
        {
            UE_LOG(LogTemp, Warning, TEXT("보스 AI 컨트롤러: 플레이어가 공격 범위 내에 있습니다!"));
            LastAttackLogTime = CurrentTime;
        }
#endif
    }

    return bInRange;
}

void ABossAIController::UpdateBossPlayerDetection()
{
    // 보스 전용 감지 로직 구현
    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ABossCharacter *Boss = Cast<ABossCharacter>(GetPawn());

    if (Boss && PlayerPawn)
    {
        // 플레이어와의 거리 계산
        float DistanceToPlayer = FVector::Dist(Boss->GetActorLocation(), PlayerPawn->GetActorLocation());

        // 보스는 항상 3000의 감지 범위를 가짐
        float DetectionRange = 2000.0f;

        // 플레이어가 보이는지 확인
        bool bCanSeePlayer = CanSeePlayerBoss();
        
        if (bCanSeePlayer)
        {
            UE_LOG(LogTemp, Warning, TEXT("보스가 플레이어를 감지하여 추적을 시작합니다. 거리: %f"), DistanceToPlayer);
        }

        // 특정 거리 이내에 플레이어가 있으면 특수 공격 가능성 설정
        bool bCanUseSpecialAttack = DistanceToPlayer <= 500.0f && bCanSeePlayer;

        // 공격 범위 내에 있는지 확인
        bool bInAttackRange = IsInAttackRange();

        if (BlackboardComponent)
        {
            BlackboardComponent->SetValueAsBool("CanUseSpecialAttack", bCanUseSpecialAttack);
            BlackboardComponent->SetValueAsBool("IsInAttackRange", bInAttackRange);
            BlackboardComponent->SetValueAsObject("TargetActor", bCanSeePlayer ? PlayerPawn : nullptr);
            BlackboardComponent->SetValueAsBool("CanSeePlayer", bCanSeePlayer);
        }

        // 보스 캐릭터의 상태 업데이트
        Boss->SetPlayerDetected(bCanSeePlayer);
        Boss->bIsChasing = bCanSeePlayer;
        Boss->UpdateMovementSpeed();
    }
}