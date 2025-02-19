#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "UObject/UObjectGlobals.h"

AEnemyAIController::AEnemyAIController()
{
    // AI 컨트롤러 기본 설정
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
    }
}

void AEnemyAIController::OnUnPossess()
{
    Super::OnUnPossess();
} 
