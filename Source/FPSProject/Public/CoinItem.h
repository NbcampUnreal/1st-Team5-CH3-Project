#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class FPSPROJECT_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ACoinItem();
	
protected:
	int32 PointValue;

	virtual void BeginPlay() override;
	virtual void ActivateItem(AActor* Activator) override;



};
