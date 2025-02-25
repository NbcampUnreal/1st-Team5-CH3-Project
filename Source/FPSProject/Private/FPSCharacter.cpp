#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "CharacterInterface.h"

// 생성자
AFPSCharacter::AFPSCharacter()
{
    // Crouch 기능 활성화
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

    PrimaryActorTick.bCanEverTick = false;

    // 스프링 암 컴포넌트 생성 및 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    // 카메라 컴포넌트 생성 및 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 이동 속도 설정
    NormalSpeed = 400.0f;
    SprintSpeedMultiplier = 2.0f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    // 체력 초기화
    Health = 100.0f;

    // 기본 이동 속도 설정
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
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Triggered, this, &AFPSCharacter::StartCrouch);
                EnhancedInput->BindAction(PlayerController->CrouchAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopCrouch);
            }

        }
    }
}



// 현재 체력을 반환
float AFPSCharacter::GetHealth() const
{
    return Health;
}

// 이동 처리
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

// 점프 시작
void AFPSCharacter::StartJump(const FInputActionValue& value)
{
    if (value.Get<bool>())
    {
        Jump();
        TakeDamage(10);
        UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), Health);
    }
}

// 점프 종료
void AFPSCharacter::StopJump(const FInputActionValue& value)
{
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

// 마우스 이동
void AFPSCharacter::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();
    AddControllerYawInput(LookInput.X);
    AddControllerPitchInput(LookInput.Y);
}

// 달리기 시작
void AFPSCharacter::StartSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

// 달리기 종료
void AFPSCharacter::StopSprint(const FInputActionValue& value)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}

// 데미지를 받았을 때 체력 감소
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

// 캐릭터 사망 처리
void AFPSCharacter::Die()
{
    if (!bIsAlive) return;

    UE_LOG(LogTemp, Warning, TEXT("Character has died."));
    bIsAlive = false;
    Destroy();
}

// 공격 동작 실행
void AFPSCharacter::Attack()
{
    UE_LOG(LogTemp, Warning, TEXT("Character attacked."));
}

// 특정 위치로 이동
void AFPSCharacter::MoveTo(FVector TargetLocation)
{
    UE_LOG(LogTemp, Warning, TEXT("Moving to location: %s"), *TargetLocation.ToString());
}

// 캐릭터의 생존 여부 반환
bool AFPSCharacter::IsAlive() const
{
    return bIsAlive;
}

// 애니메이션 실행
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
        // 1인칭 설정
        SpringArmComp->TargetArmLength = 0.0f;
        CameraComp->bUsePawnControlRotation = true;
        SpringArmComp->bUsePawnControlRotation = true;
        bUseControllerRotationYaw = true;

        GetMesh()->SetOwnerNoSee(true);
    }
    else
    {
        // 3인칭으로 변경될 때 현재 카메라 방향을 캐릭터 방향으로 반영
        AController* PlayerController = GetController();
        if (PlayerController)
        {
            PlayerController->SetControlRotation(GetActorRotation()); // 현재 캐릭터 회전값으로 설정
        }

        // 3인칭 설정
        SpringArmComp->TargetArmLength = 300.0f;
        CameraComp->bUsePawnControlRotation = false;
        SpringArmComp->bUsePawnControlRotation = true;
        bUseControllerRotationYaw = true;

        GetMesh()->SetOwnerNoSee(false);
    }

    UE_LOG(LogTemp, Warning, TEXT("Switched to %s"), bIsFirstPerson ? TEXT("First Person") : TEXT("Third Person"));
}
void AFPSCharacter::StartCrouch(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Crouch Start Pressed!"));

    if (GetCharacterMovement() && !bIsCrouched)
    {
        UE_LOG(LogTemp, Warning, TEXT("Crouch Started!"));

        // 크라우치 실행
        Crouch();
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * 0.5f;
    }
}

void AFPSCharacter::StopCrouch(const FInputActionValue& Value)
{
    UE_LOG(LogTemp, Warning, TEXT("Crouch Stop Pressed!"));

    if (GetCharacterMovement() && bIsCrouched)
    {
        UE_LOG(LogTemp, Warning, TEXT("Crouch Stopped!"));

        // 크라우치 해제
        UnCrouch();
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}
