#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "BulletItem.generated.h"

UCLASS()
class FPSPROJECT_API ABulletItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ABulletItem();

protected:
	virtual void ActivateItem(AActor* Activator) override;

private:
	// 아이템이 증가시킬 총알 개수
	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 AmmoAmount = 10;
};
