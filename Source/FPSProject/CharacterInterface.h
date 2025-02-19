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
    // Ž�� ���
    virtual void OnDetected(AActor* DetectedActor) = 0; // AI�� �÷��̾ �߰����� ��
    virtual void OnLostSight(AActor* LostActor) = 0;   // AI�� �÷��̾ ������ ��

    // �⺻ ĳ���� �ൿ
    virtual void TakeDamage(float DamageAmount) = 0;   // ���� �Ա�
    virtual void Die() = 0;                            // ��� ó��

};
