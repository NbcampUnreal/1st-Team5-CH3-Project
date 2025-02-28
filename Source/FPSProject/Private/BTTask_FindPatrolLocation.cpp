#include "BTTask_FindPatrolLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
    NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러와 폰 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
        return EBTNodeResult::Failed;

    APawn* AIPawn = AIController->GetPawn();
    if (!AIPawn)
        return EBTNodeResult::Failed;

    // 네비게이션 시스템 가져오기
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
    if (!NavSystem)
        return EBTNodeResult::Failed;

    // 랜덤한 위치 찾기
    FNavLocation RandomLocation;
    if (NavSystem->GetRandomReachablePointInRadius(AIPawn->GetActorLocation(), PatrolRadius, RandomLocation))
    {
        // Blackboard에 위치 저장
        OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolLocation", RandomLocation.Location);
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
