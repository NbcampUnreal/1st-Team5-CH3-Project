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
  
        virtual void TakeDamage(float DamageAmount) = 0;  // 피격 처리
        virtual void Die() = 0;  // 사망 처리
        virtual void Attack() = 0;  // 공격 실행
        virtual void MoveTo(FVector TargetLocation) = 0;  // 목표 위치로 이동
        virtual void PlayAnimation(UAnimMontage* Animation) = 0;  // 애니메이션 실행
        virtual bool IsAlive() const = 0;  // 생존 여부 체크

};
