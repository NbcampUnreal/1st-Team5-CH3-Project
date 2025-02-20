#include "BTDecorator_CanSeePlayer.h"
#include "AIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CanSeePlayer::UBTDecorator_CanSeePlayer()
{
    NodeName = TEXT("Can See Player");
}

bool UBTDecorator_CanSeePlayer::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    // Blackboard에서 CanSeePlayer 값을 가져옴
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
        return false;

    return BlackboardComp->GetValueAsBool("CanSeePlayer");
}
