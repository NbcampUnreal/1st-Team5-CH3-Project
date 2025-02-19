#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "UObject/UObjectGlobals.h"
#include "Perception/AIPerceptionSystem.h"

AEnemyAIController::AEnemyAIController()
{
    // AI 컨트롤러 기본 설정
    
    // AI Perception Component 생성
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
    
    // 시야 감지 설정 생성
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
    
    // 기본 시야 설정
    // TODO: 세부 설정 추가
    if (SightConfig)
    {
        AIPerceptionComponent->ConfigureSense(*SightConfig);
        AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    }
}

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    // 감지 이벤트 등록
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnPerceptionUpdated);
    }
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if (AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(InPawn))
    {
        UE_LOG(LogTemp, Log, TEXT("AI Controller possessed Enemy Character"));
    }
}

void AEnemyAIController::OnUnPossess()
{
    Super::OnUnPossess();
}

void AEnemyAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    // TODO: FPSCharacter 클래스 구현 후 추가
    /** 
    for (AActor* Actor : UpdatedActors)
    {
        if (AFPSCharacter* Player = Cast<AFPSCharacter>(Actor))
        {
         //   // 플레이어 감지 시 처리
        }
    }
    */
} 
