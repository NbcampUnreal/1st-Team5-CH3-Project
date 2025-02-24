#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "FPSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionTypes.h"

AEnemyAIController::AEnemyAIController()
{
    // AI Perception Component 생성 및 설정
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    // 시야 설정
    if (SightConfig)
    {
        // 기본 시야 설정 추가
        SightConfig->SightRadius = 3000.0f;
        SightConfig->LoseSightRadius = 3500.0f;
        SightConfig->PeripheralVisionAngleDegrees = 120.0f;
        
        // 감지 설정
        SightConfig->SetMaxAge(5.0f);

        // 감지 대상 설정
        SightConfig->DetectionByAffiliation.bDetectEnemies = true;
        SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
        SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

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
