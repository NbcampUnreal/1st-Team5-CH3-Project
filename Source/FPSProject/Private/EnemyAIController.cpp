#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "FPSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionTypes.h"

AEnemyAIController::AEnemyAIController()
{
    // AI Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    
    // Sight Config 설정
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    if (SightConfig)
    {
        SightConfig->SightRadius = 1000.0f;
        SightConfig->LoseSightRadius = 1500.0f;
        SightConfig->PeripheralVisionAngleDegrees = 90.0f;
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    }
    
    // Perception Component에 Sight Config 추가
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
    }

    // BT Component와 BB Component 생성
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
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
            if (AIPerceptionComponent)
            {
                // GetCurrentlyPerceivedActors 사용
                TArray<AActor*> PerceivedActors;
                AIPerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
                bCanSeePlayer = PerceivedActors.Contains(Player);
                
                UE_LOG(LogTemp, Warning, TEXT("Can See Player: %s"), 
                    bCanSeePlayer ? TEXT("True") : TEXT("False"));
            }
            
            if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn()))
            {
                Enemy->bIsChasing = bCanSeePlayer;
            }
            
           if (BlackboardComponent)
            {
                BlackboardComponent->SetValueAsObject("TargetActor", bCanSeePlayer ? Player : nullptr);
                BlackboardComponent->SetValueAsBool("CanSeePlayer", bCanSeePlayer);
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
