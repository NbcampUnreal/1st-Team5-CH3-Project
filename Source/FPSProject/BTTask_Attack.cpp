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

    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 새로운 버스트 시작
    if (CurrentBurstTarget == 0)
    {
        // 패턴 쿨타임 체크
        if (CurrentTime - LastAttackTime < PatternCooldown)
        {
            return EBTNodeResult::Failed;
        }

        CurrentBurstTarget = FMath::RandRange(MinBurstCount, MaxBurstCount);
        UE_LOG(LogTemp, Warning, TEXT("New burst started, target count: %d"), CurrentBurstTarget);
    }

    // 버스트 쿨타임 체크
    if (CurrentTime - LastAttackTime < BurstCooldown)
    {
        return EBTNodeResult::Failed;
    }

    // 공격 실행
    UE_LOG(LogTemp, Warning, TEXT("Executing Attack! Bursts remaining: %d"), CurrentBurstTarget);
    EnemyCharacter->Attack();  // 여기서 애니메이션 재생
    
    // 애니메이션 길이만큼 대기하도록 수정
    float AnimationLength = 1.0f;  // 나중에 실제 애니메이션 길이로 대체
    LastAttackTime = CurrentTime + AnimationLength;
    CurrentBurstTarget--;

    // 버스트 완료 체크
    if (CurrentBurstTarget == 0)
    {
        LastAttackTime = CurrentTime;
        
        EnemyCharacter->PlayAnimation(ShootMontage);  
        
        UE_LOG(LogTemp, Warning, TEXT("Burst complete, playing shoot animation"));
    }

    return EBTNodeResult::Succeeded;
}
