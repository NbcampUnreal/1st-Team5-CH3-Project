// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSPROJECT_API ICharacterInterface
{
	GENERATED_BODY()


	
public:
    // 탐지 기능
    virtual void OnDetected(AActor* DetectedActor) = 0; // AI가 플레이어를 발견했을 때
    virtual void OnLostSight(AActor* LostActor) = 0;   // AI가 플레이어를 놓쳤을 때

    // 기본 캐릭터 행동
    virtual void TakeDamage(float DamageAmount) = 0;   // 피해 입기
    virtual void Die() = 0;                            // 사망 처리

};
