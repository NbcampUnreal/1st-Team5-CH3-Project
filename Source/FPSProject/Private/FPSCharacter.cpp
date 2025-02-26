#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "CharacterInterface.h"

AFPSCharacter::AFPSCharacter()
{
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    PrimaryActorTick.bCanEverTick = false;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 1인칭 캐릭터 모델
    FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    FirstPersonMesh->SetupAttachment(CameraComp);
    FirstPersonMesh->bOwnerNoSee = true; // 3인칭일 때 숨김


    NormalSpeed = 400.0f;
    SprintSpeedMultiplier = 2.0f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    Health = 100.0f;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController());
        if (PlayerController)
        {
            if (PlayerController->MoveAction)
            {
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
            }
            if (PlayerController->JumpAction)
            {
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &AFPSCharacter::StartJump);
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopJump);
            }
            if (PlayerController->LookAction)
            {
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
            }
            if (PlayerController->SprintAction)
            {
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered, this, &AFPSCharacter::StartSprint);
                EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopSprint);
            }
            if (PlayerController->Viewpoint_TransformationAction) // 여기가 중요!
            {
                UE_LOG(LogTemp, Warning, TEXT("Binding Viewpoint_Transformation Action!"));
                EnhancedInput->BindAction(PlayerController->Viewpoint_TransformationAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Viewpoint_Transformation);
            }
            if (PlayerController->CrouchAction)
            {
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Started, this, &AFPSCharacter::StartCrouch);
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopCrouch);
            }

        }
    }
}


float AFPSCharacter::GetHealth() const
{
    return Health;
}

void AFPSCharacter::Move(const FInputActionValue& value)
{
    if (!Controller) return;

    const FVector2D MoveInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void AFPSCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
    {
        Jump();
    }
}

void AFPSCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

void AFPSCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

void AFPSCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
        SetCharacterState(ECharacterState::Sprinting);  // AI 감지용 상태 변경
    }
}

void AFPSCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
        SetCharacterState(ECharacterState::Normal);     // AI 감지용 상태 변경
    }
}


void AFPSCharacter::TakeDamage(float DamageAmount)
{
    if (!bIsAlive) return;

    Health -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("Character took damage: %f, Current Health: %f"), DamageAmount, Health);

    if (Health <= 0)
    {
        Die();
    }
}


void AFPSCharacter::Die()
{
    if (!bIsAlive) return;

    bIsAlive = false;
    SetCharacterState(ECharacterState::Dead);          // AI 감지용 상태 변경
    GetCharacterMovement()->DisableMovement();
    DisableInput(Cast<APlayerController>(GetController()));

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        PlayerController->SetIgnoreLookInput(true);
        PlayerController->SetIgnoreMoveInput(true);
    }

    // 1인칭 모드에서 죽으면 자동으로 3인칭으로 전환
    if (!bIsFirstPerson)
    {
        Viewpoint_Transformation(); // 3인칭 모드로 변경
    }

    // 죽는 애니메이션 재생
    if (DeathMontage)
    {
        float MontageDuration = PlayAnimMontage(DeathMontage);
        GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AFPSCharacter::DestroyCharacter, MontageDuration, false);
    }
    else
    {
        // 애니메이션 없이 즉시 삭제
        DestroyCharacter();
    }

    UE_LOG(LogTemp, Warning, TEXT("Character Died and switched to Third-Person View."));
}




//   
void AFPSCharacter::Attack()
{
    UE_LOG(LogTemp, Warning, TEXT("Character attacked."));
}

// 특정 위치로 이동
void AFPSCharacter::MoveTo(FVector TargetLocation)
{
    UE_LOG(LogTemp, Warning, TEXT("Moving to location: %s"), *TargetLocation.ToString());
}

// 캐릭터의 생존 상태 반환
bool AFPSCharacter::IsAlive() const
{
    return bIsAlive;
}

// 애니메이션 재생
void AFPSCharacter::PlayAnimation(UAnimMontage* Animation)
{
    if (Animation)
    {
        UE_LOG(LogTemp, Warning, TEXT("Playing animation."));
        PlayAnimMontage(Animation);
    }
}

void AFPSCharacter::Viewpoint_Transformation()
{
    UE_LOG(LogTemp, Warning, TEXT("Viewpoint_Transformation function called!"));

    bIsFirstPerson = !bIsFirstPerson;

    if (!bIsFirstPerson)
    {
        // 1인칭 모드
        SpringArmComp->TargetArmLength = 0.0f;
        CameraComp->bUsePawnControlRotation = true;
        SpringArmComp->bUsePawnControlRotation = true;
        bUseControllerRotationYaw = true;

        // 1인칭 모델 보이기, 3인칭 모델 숨기기
        FirstPersonMesh->SetOwnerNoSee(false);
        GetMesh()->SetOwnerNoSee(true);

        // 3인칭 모델 그림자 제거
        GetMesh()->bCastDynamicShadow = false;
        GetMesh()->CastShadow = false;
    }
    else
    {
        // 3인칭 모드
        AController* PlayerController = GetController();
        if (PlayerController)
        {
            PlayerController->SetControlRotation(GetActorRotation());
        }

        SpringArmComp->TargetArmLength = 300.0f;
        CameraComp->bUsePawnControlRotation = false;
        SpringArmComp->bUsePawnControlRotation = true;
        bUseControllerRotationYaw = true;

        // 1인칭 모델 숨기기, 3인칭 모델 보이기
        FirstPersonMesh->SetOwnerNoSee(true);
        GetMesh()->SetOwnerNoSee(false);

        // 3인칭 모델 그림자 다시 활성화
        GetMesh()->bCastDynamicShadow = true;
        GetMesh()->CastShadow = true;
    }

    UE_LOG(LogTemp, Warning, TEXT("Switched to %s"), bIsFirstPerson ? TEXT("First Person") : TEXT("Third Person"));
}




void AFPSCharacter::StartCrouch(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Crouch Start Pressed!"));

    if (GetCharacterMovement() && !bIsCrouched)
    {
        UE_LOG(LogTemp, Warning, TEXT("Crouch Started!"));

        // 크라우치 시작
        Crouch();
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * 0.5f;
        SetCharacterState(ECharacterState::Crouching);     // AI 감지용 상태 변경
    }
}

void AFPSCharacter::StopCrouch(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Crouch Stop Pressed!"));

    if (GetCharacterMovement() && bIsCrouched)
    {
        UE_LOG(LogTemp, Warning, TEXT("Crouch Stopped!"));

        // 크라우치 종료
        UnCrouch();
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
        SetCharacterState(ECharacterState::Normal);        // AI 감지용 상태 변경
    }
}

void AFPSCharacter::DestroyCharacter()
{
    Destroy();
}

void AFPSCharacter::SetCharacterState(ECharacterState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        HandleStateChange(NewState);
        OnStateChanged(NewState);
    }
}

void AFPSCharacter::HandleStateChange(ECharacterState NewState)
{
    switch (NewState)
    {
        case ECharacterState::Normal:
            GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
            break;
            
        case ECharacterState::Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
            break;
            
        case ECharacterState::Crouching:
            GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * 0.5f;
            break;
            
        case ECharacterState::Dead:
            GetCharacterMovement()->StopMovementImmediately();
            break;
    }

    // 상태 변경 로그
    UE_LOG(LogTemp, Warning, TEXT("Character State Changed to: %s"), 
        *UEnum::GetValueAsString(NewState));
}

