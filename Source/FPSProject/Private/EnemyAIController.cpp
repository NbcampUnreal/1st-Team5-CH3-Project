#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BossCharacter.h"
#include "FPSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AEnemyAIController::AEnemyAIController()
{
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

    PrimaryActorTick.bCanEverTick = true;  // Tick 활성화
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(InPawn))
    {
        UE_LOG(LogTemp, Log, TEXT("AI Controller possessed Enemy Character"));

        // Behavior Tree 실행
        if (BehaviorTree)
        {
            // Blackboard 초기화
            BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
            
            // Behavior Tree 실행
            BehaviorTreeComponent->StartTree(*BehaviorTree);
        }
    }
}

void AEnemyAIController::OnUnPossess()
{
    Super::OnUnPossess();
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 매 Tick마다 플레이어 감지 상태 업데이트
    UpdatePlayerDetection();
}

void AEnemyAIController::UpdatePlayerDetection()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AFPSCharacter* Player = Cast<AFPSCharacter>(PlayerPawn);
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    
    if (Enemy && Player)
    {
        // 플레이어 상태에 따라 감지 범위 업데이트
        Enemy->UpdateDetectionRangeForPlayerState(Player);
    }

    bool bCanSeePlayer = CanSeePlayer();
    
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsObject("TargetActor", bCanSeePlayer ? PlayerPawn : nullptr);
        BlackboardComponent->SetValueAsBool("CanSeePlayer", bCanSeePlayer);
    }
    
    if (Enemy)
    {
        // 플레이어 감지 상태 업데이트
        Enemy->SetPlayerDetected(bCanSeePlayer);
        
        // 추격 상태 업데이트
        Enemy->bIsChasing = bCanSeePlayer;
        Enemy->UpdateMovementSpeed();
    }
}

bool AEnemyAIController::CanSeePlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
    
    if (!PlayerPawn || !Enemy)
    {
        return false;
    }

    // 보스 캐릭터인 경우 감지 범위를 2500으로 설정
    float DetectionRange = Enemy->GetDetectionRange();
    if (Enemy->IsA(ABossCharacter::StaticClass()))
    {
        DetectionRange = 2500.0f;
    }

    // 거리 계산
    float Distance = FVector::Dist(Enemy->GetActorLocation(), PlayerPawn->GetActorLocation());
    
    // 감지 범위 내에 있는지 확인 (360도 원형 감지)
    if (Distance <= DetectionRange)
    {
        // 시야선 체크 (벽이나 장애물 체크)
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Enemy);
        
        FVector StartLocation = Enemy->GetActorLocation();
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
        
        return bHasLineOfSight;
    }
    
    return false;
}

bool AEnemyAIController::IsInAttackRange()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(GetPawn());
    
    if (!PlayerPawn || !EnemyChar) return false;
    
    // 거리 계산
    float Distance = FVector::Dist(EnemyChar->GetActorLocation(), PlayerPawn->GetActorLocation());
    
    // 공격 범위 내에 있는지 확인
    if (Distance <= EnemyChar->GetAttackRange())
    {
        // 부채꼴 각도 확인 (전방 60도 내에 있는지) - 시각화 및 기본 체크용
        FVector EnemyForward = EnemyChar->GetActorForwardVector();
        FVector DirectionToPlayer = (PlayerPawn->GetActorLocation() - EnemyChar->GetActorLocation()).GetSafeNormal();
        
        float DotProduct = FVector::DotProduct(EnemyForward, DirectionToPlayer);
        float AngleRad = FMath::Acos(DotProduct);
        float AngleDeg = FMath::RadiansToDegrees(AngleRad);
        
        // 전방 60도 내에 있으면
        if (AngleDeg <= 30.0f)  // 60도 부채꼴이므로 중심에서 30도
        {
            // 직선 레이캐스트로 실제 공격 가능 여부 확인
            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(EnemyChar);
            
            FVector StartLocation = EnemyChar->GetActorLocation();
            StartLocation.Z += 50.0f;  // 총구 높이 조정
            
            FVector EndLocation = PlayerPawn->GetActorLocation();
            EndLocation.Z += 50.0f;    // 플레이어 중심점
            
            // 총알이 날아갈 경로에 장애물이 없는지 확인
            bool bHasLineOfSight = !GetWorld()->LineTraceSingleByChannel(
                HitResult, 
                StartLocation, 
                EndLocation, 
                ECC_Visibility, 
                QueryParams
            );

            if (bHasLineOfSight)
            {
                UE_LOG(LogTemp, Log, TEXT("Enemy can attack player - Distance: %f, Angle: %f"), 
                    Distance, AngleDeg);
                return true;
            }
        }
    }
    
    return false;
}
