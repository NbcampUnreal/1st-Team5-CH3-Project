// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FPSProjectile.h"
#include "GameFramework/PlayerController.h" //  APlayerController 사용
#include "Kismet/GameplayStatics.h" //  DeprojectScreenToWorld 사용
#include "FPSCharacter.generated.h"

UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    //이 캐릭터의 프로퍼티에 적용되는 디폴트값 설정
    AFPSCharacter();

protected:
    virtual void BeginPlay() override;

    // 스폰할 발사체 클래스입니다.
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
    void Fire(); //  HUD 에임과 일치하는 Fire 함수

    // FPS 카메라
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FPSCameraComponent;

    // 일인칭 메시(팔)로, 소유 플레이어에게만 보입니다.
    UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
    USkeletalMeshComponent* FPSMesh;

    // 카메라 위치로부터의 총구 오프셋입니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    FVector MuzzleOffset;
};
