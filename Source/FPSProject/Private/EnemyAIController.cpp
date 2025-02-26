#include "EnemyAIController.h"
#include "EnemyCharacter.h"
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
    
    // 디버그 시각화
    #if WITH_EDITOR
    DrawDebugDetectionRange();
    #endif
}

void AEnemyAIController::UpdatePlayerDetection()
{
    bool bCanSeePlayer = CanSeePlayer();
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    
    if (BlackboardComponent)
    {
        BlackboardComponent->SetValueAsObject("TargetActor", bCanSeePlayer ? PlayerPawn : nullptr);
        BlackboardComponent->SetValueAsBool("CanSeePlayer", bCanSeePlayer);
    }
    
    // Enemy 상태 업데이트
    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
    {
        Enemy->bIsChasing = bCanSeePlayer;
        Enemy->UpdateMovementSpeed();
    }
}

bool AEnemyAIController::CanSeePlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(GetPawn());
    
    if (!PlayerPawn || !EnemyChar) return false;
    
    // 거리 계산
    float Distance = FVector::Dist(EnemyChar->GetActorLocation(), PlayerPawn->GetActorLocation());
    
    // 감지 범위 내에 있는지 확인 (360도 원형 감지)
    if (Distance <= EnemyChar->GetDetectionRange())
    {
        // 시야선 체크 (벽이나 장애물 체크)
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(EnemyChar);
        
        FVector StartLocation = EnemyChar->GetActorLocation();
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
        // 부채꼴 각도 확인 (전방 60도 내에 있는지)
        FVector EnemyForward = EnemyChar->GetActorForwardVector();
        FVector DirectionToPlayer = (PlayerPawn->GetActorLocation() - EnemyChar->GetActorLocation()).GetSafeNormal();
        
        float DotProduct = FVector::DotProduct(EnemyForward, DirectionToPlayer);
        float AngleRad = FMath::Acos(DotProduct);
        float AngleDeg = FMath::RadiansToDegrees(AngleRad);
        
        // 전방 60도 내에 있으면 공격 가능
        if (AngleDeg <= 30.0f)  // 60도 부채꼴이므로 중심에서 30도
        {
            return true;
        }
    }
    
    return false;
}

void AEnemyAIController::DrawDebugDetectionRange()
{
    AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(GetPawn());
    if (!EnemyChar) return;
    
    // 감지 범위 시각화 (흰색 원)
    DrawDebugSphere(
        GetWorld(),
        EnemyChar->GetActorLocation(),
        EnemyChar->GetDetectionRange(),
        32,  // 세그먼트 수
        FColor::White,
        false,  // 지속적
        -1.0f,  // 수명
        0,
        1.0f  // 두께
    );
    
    // 공격 범위 시각화 (빨간색 부채꼴)
    float AttackRange = EnemyChar->GetAttackRange();
    FVector EnemyLocation = EnemyChar->GetActorLocation();
    FVector EnemyForward = EnemyChar->GetActorForwardVector();
    FVector EnemyRight = EnemyChar->GetActorRightVector();
    
    // 부채꼴 그리기
    for (float Angle = -30.0f; Angle <= 30.0f; Angle += 5.0f)
    {
        float AngleRad = FMath::DegreesToRadians(Angle);
        FVector Direction = EnemyForward * FMath::Cos(AngleRad) + EnemyRight * FMath::Sin(AngleRad);
        
        DrawDebugLine(
            GetWorld(),
            EnemyLocation,
            EnemyLocation + Direction * AttackRange,
            FColor::Red,
            false,
            -1.0f,
            0,
            1.0f
        );
    }
    
    // 부채꼴 외곽선 그리기
    int32 NumSegments = 12;
    float AngleStep = 60.0f / NumSegments;  // 60도 부채꼴
    
    for (int32 i = 0; i <= NumSegments; i++)
    {
        float Angle = -30.0f + i * AngleStep;
        float AngleRad = FMath::DegreesToRadians(Angle);
        FVector Direction = EnemyForward * FMath::Cos(AngleRad) + EnemyRight * FMath::Sin(AngleRad);
        FVector EndPoint = EnemyLocation + Direction * AttackRange;
        
        if (i > 0)
        {
            float PrevAngle = -30.0f + (i-1) * AngleStep;
            float PrevAngleRad = FMath::DegreesToRadians(PrevAngle);
            FVector PrevDirection = EnemyForward * FMath::Cos(PrevAngleRad) + EnemyRight * FMath::Sin(PrevAngleRad);
            FVector PrevEndPoint = EnemyLocation + PrevDirection * AttackRange;
            
            DrawDebugLine(
                GetWorld(),
                PrevEndPoint,
                EndPoint,
                FColor::Red,
                false,
                -1.0f,
                0,
                1.0f
            );
        }
    }
}
