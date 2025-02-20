#include "BTDecorator_IsInAttackRange.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
    NodeName = TEXT("Is In Attack Range");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // AI 컨트롤러와 폰 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
        return false;

    AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(AIController->GetPawn());
    if (!EnemyCharacter)
        return false;

    // 타겟 액터 가져오기
    AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetActor"));
    if (!TargetActor)
        return false;

    // 거리 계산
    float Distance = FVector::Distance(EnemyCharacter->GetActorLocation(), TargetActor->GetActorLocation());
    
    // 공격 범위 내에 있는지 확인
    return Distance <= EnemyCharacter->GetAttackRange();
}
