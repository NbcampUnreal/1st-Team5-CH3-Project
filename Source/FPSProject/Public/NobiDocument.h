#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "NobiDocument.generated.h"

UCLASS()
class FPSPROJECT_API ANobiDocument : public ABaseItem
{
	GENERATED_BODY()
	
public:	
	ANobiDocument();

protected:
	virtual void BeginPlay() override;
	virtual void ActivateItem(AActor* Activator) override;
	virtual void Tick(float DeltaTime) override;

	float RotationSpeed;

};
