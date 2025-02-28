#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanSeePlayer.generated.h"

UCLASS()
class FPSPROJECT_API UBTDecorator_CanSeePlayer : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanSeePlayer();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
