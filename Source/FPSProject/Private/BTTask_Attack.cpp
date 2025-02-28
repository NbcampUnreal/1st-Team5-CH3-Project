#include "BTTask_Attack.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!EnemyCharacter) return EBTNodeResult::Failed;

    // 공격 실행
    EnemyCharacter->Attack();

    return EBTNodeResult::Succeeded;
}
