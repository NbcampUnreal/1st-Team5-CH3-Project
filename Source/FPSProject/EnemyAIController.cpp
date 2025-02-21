#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "FPSCharacter.h"
#include "UObject/UObjectGlobals.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"

AEnemyAIController::AEnemyAIController()
{
    // AI 컨트롤러 기본 설정
    
    // AI Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    
    // 시야 감지 설정 생성
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    
    // 시야 설정
    if (SightConfig)
    {
        // 기본 시야 설정 추가
        SightConfig->SightRadius = 3000.0f;
        SightConfig->LoseSightRadius = 3500.0f;
        SightConfig->PeripheralVisionAngleDegrees = 120.0f;
        
        // 감지 대상 설정
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        
        // 감지 설정
        SightConfig->SetMaxAge(5.0f);

        // AI Perception Component에 설정 등록
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
    }

    // Behavior Tree 컴포넌트 생성
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    
    // Blackboard 컴포넌트 생성
    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
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

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    bool bCanSeePlayer = false;
    
    for (AActor* Actor : UpdatedActors)
    {
        if (AFPSCharacter* Player = Cast<AFPSCharacter>(Actor))
        {
            // 디버그 로그 추가
            UE_LOG(LogTemp, Warning, TEXT("Found FPSCharacter in UpdatedActors"));
            
            if (AIPerceptionComponent)
            {
                TArray<AActor*> PerceivedActors;
                AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
                
                // 디버그 로그 추가
                bCanSeePlayer = PerceivedActors.Contains(Player);
                UE_LOG(LogTemp, Warning, TEXT("Can See Player: %s"), bCanSeePlayer ? TEXT("True") : TEXT("False"));
            }
            
            if (BlackboardComponent)
            {
                BlackboardComponent->SetValueAsObject("TargetActor", bCanSeePlayer ? Player : nullptr);
                BlackboardComponent->SetValueAsBool("CanSeePlayer", bCanSeePlayer);
                
                // 플레이어를 향해 이동하도록 위치 설정 추가
                if (bCanSeePlayer)
                {
                    BlackboardComponent->SetValueAsVector("PatrolLocation", Player->GetActorLocation());
                }
                
                // 디버그 로그 추가
                UE_LOG(LogTemp, Warning, TEXT("Updated Blackboard Values"));
            }
            break;
        }
    }
    
    // 플레이어를 못 볼 때 상태 업데이트
    if (!bCanSeePlayer && BlackboardComponent)
    {
        BlackboardComponent->SetValueAsObject("TargetActor", nullptr);
        BlackboardComponent->SetValueAsBool("CanSeePlayer", false);
    }
}
