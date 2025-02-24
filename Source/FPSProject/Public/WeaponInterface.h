// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};


class FPSPROJECT_API IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void OnItemOverlap(AActor* OverlapActor) = 0;
	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
	
};
