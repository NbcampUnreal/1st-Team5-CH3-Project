#include "FPSCharacter.h"
#include "FPSPlayerController.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 300.0f;
    SpringArmComp->bUsePawnControlRotation = true;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    NormalSpeed = 300.0f;
    SprintSpeedMultiplier = 2.0f;
    SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

    GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced InputComponent�� ĳ����
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA�� �������� ���� ���� ���� ���� Controller�� AFPSPlayerController�� ĳ����
        if (AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                // IA_Move �׼� Ű�� "Ű�� ������ �ִ� ����" Move() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AFPSCharacter::Move
                );
            }

            if (PlayerController->JumpAction)
            {
                // IA_Jump �׼� Ű�� "Ű�� ������ �ִ� ����" StartJump() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AFPSCharacter::StartJump
                );

                // IA_Jump �׼� Ű���� "���� �� ����" StopJump() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &AFPSCharacter::StopJump
                );
            }

            if (PlayerController->LookAction)
            {
                // IA_Look �׼� ���콺�� "������ ��" Look() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AFPSCharacter::Look
                );
            }

            if (PlayerController->SprintAction)
            {
                // IA_Sprint �׼� Ű�� "������ �ִ� ����" StartSprint() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AFPSCharacter::StartSprint
                );
                // IA_Sprint �׼� Ű���� "���� �� ����" StopSprint() ȣ��
                EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Completed,
                    this,
                    &AFPSCharacter::StopSprint
                );
            }
        }
    }
}

void AFPSCharacter::Move(const FInputActionValue& value)
{
    // ��Ʈ�ѷ��� �־�� ���� ����� ����
    if (!Controller) return;

    // Value�� Axis2D�� ������ IA_Move�� �Է°� (WASD)�� ��� ����
// ��) (X=1, Y=0) �� ���� / (X=-1, Y=0) �� ���� / (X=0, Y=1) �� ������ / (X=0, Y=-1) �� ����
    const FVector2D MoveInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        // ĳ���Ͱ� �ٶ󺸴� ����(����)���� X�� �̵�
        AddMovementInput(GetActorForwardVector(), MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        // ĳ������ ������ �������� Y�� �̵�
        AddMovementInput(GetActorRightVector(), MoveInput.Y);
    }
}

void AFPSCharacter::StartJump(const FInputActionValue& value)
{
    // Jump �Լ��� Character�� �⺻ ����
    if (value.Get<bool>())
    {
        Jump();
    }
}

void AFPSCharacter::StopJump(const FInputActionValue& value)
{
    // StopJumping �Լ��� Character�� �⺻ ����
    if (!value.Get<bool>())
    {
        StopJumping();
    }
}

void AFPSCharacter::Look(const FInputActionValue& value)
{
    // ���콺�� X, Y �������� 2D ������ ������
    FVector2D LookInput = value.Get<FVector2D>();

    // X�� �¿� ȸ�� (Yaw), Y�� ���� ȸ�� (Pitch)
    // �¿� ȸ��
    AddControllerYawInput(LookInput.X);
    // ���� ȸ��
    AddControllerPitchInput(LookInput.Y);
}

void AFPSCharacter::StartSprint(const FInputActionValue& value)
{
    // Shift Ű�� ���� ���� �� �Լ��� ȣ��ȴٰ� ����
    // ������Ʈ �ӵ��� ����
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    }
}

void AFPSCharacter::StopSprint(const FInputActionValue& value)
{
    // Shift Ű�� �� ���� �� �Լ��� ȣ��
   // ���� �ӵ��� ����
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
    }
}