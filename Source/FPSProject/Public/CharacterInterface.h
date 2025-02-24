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
  
        virtual void TakeDamage(float DamageAmount) = 0;  // �ǰ� ó��
        virtual void Die() = 0;  // ��� ó��
        virtual void Attack() = 0;  // ���� ����
        virtual void MoveTo(FVector TargetLocation) = 0;  // ��ǥ ��ġ�� �̵�
        virtual void PlayAnimation(UAnimMontage* Animation) = 0;  // �ִϸ��̼� ����
        virtual bool IsAlive() const = 0;  // ���� ���� üũ

};
