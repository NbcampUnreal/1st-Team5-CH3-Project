// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FPSProjectile.h"
#include "GameFramework/PlayerController.h" //  APlayerController ���
#include "Kismet/GameplayStatics.h" //  DeprojectScreenToWorld ���
#include "FPSCharacter.generated.h"

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    //�� ĳ������ ������Ƽ�� ����Ǵ� ����Ʈ�� ����
    AFPSCharacter();

protected:
    virtual void BeginPlay() override;

    // ������ �߻�ü Ŭ�����Դϴ�.
    UPROPERTY(EditDefaultsOnly, Category = Projectile)
    TSubclassOf<class AFPSProjectile> ProjectileClass;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    void MoveForward(float Value);

    UFUNCTION()
    void MoveRight(float Value);

    UFUNCTION()
    void StartJump();

    UFUNCTION()
    void StopJump();

    UFUNCTION()
    void Fire(); //  HUD ���Ӱ� ��ġ�ϴ� Fire �Լ�

    // FPS ī�޶�
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FPSCameraComponent;

    // ����Ī �޽�(��)��, ���� �÷��̾�Ը� ���Դϴ�.
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    // ī�޶� ��ġ�κ����� �ѱ� �������Դϴ�.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;
};
