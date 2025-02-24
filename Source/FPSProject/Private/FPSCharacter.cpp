#include "FPSCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// ����Ʈ�� ����
AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // ����Ī ī�޶� ������Ʈ�� �����մϴ�.
    FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    check(FPSCameraComponent != nullptr);
    FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));
    FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
    FPSCameraComponent->bUsePawnControlRotation = true;

    FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
    check(FPSMesh != nullptr);
    FPSMesh->SetOnlyOwnerSee(true);
    FPSMesh->SetupAttachment(FPSCameraComponent);
    FPSMesh->bCastDynamicShadow = false;
    FPSMesh->CastShadow = false;

    GetMesh()->SetOwnerNoSee(true);
}

// ���� ���� �Ǵ� ���� �� ȣ��
void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();
    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
}

// �����Ӹ��� ȣ��
void AFPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// �Լ� ����� �Է¿� ���ε��ϱ� ���� ȣ��
void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::StopJump);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
}

void AFPSCharacter::MoveForward(float Value)
{
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
    AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value)
{
    FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
    AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump()
{
    bPressedJump = true;
}

void AFPSCharacter::StopJump()
{
    bPressedJump = false;
}

void AFPSCharacter::Fire()
{
    if (ProjectileClass)
    {
        // �߻� ��ġ ���: ī�޶��� ��ġ�� ȸ��
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);

        // MuzzleOffset�� �����Ͽ� ī�޶� �տ��� �߻�ü�� �߻�
        MuzzleOffset.Set(100.0f, 0.0f, 0.0f);
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

        // �߻�ü �߻� ����
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += 10.0f;

        // ȭ�� �߾��� ũ�ν��� �������� �߻�
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (PlayerController)
        {
            FVector MouseWorldLocation, MouseWorldDirection;
            float ScreenCenterX = GEngine->GameViewport->Viewport->GetSizeXY().X * 0.5f;  // ȭ�� �߾� X��ǥ
            float ScreenCenterY = GEngine->GameViewport->Viewport->GetSizeXY().Y * 0.5f;  // ȭ�� �߾� Y��ǥ

            // ȭ�� �߾��� ���� ��ǥ�� ��ȯ
            if (PlayerController->DeprojectScreenPositionToWorld(ScreenCenterX, ScreenCenterY, MouseWorldLocation, MouseWorldDirection))
            {
                // ���� ��ǥ�� ���� ���͸� ������ �߻�ü�� �������� ���
                FVector LaunchDirection = MouseWorldDirection;  // ȭ���� �߾��� �������� ���忡�� ������ ���մϴ�.

                // �߻�ü ����
                UWorld* World = GetWorld();
                if (World)
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = this;
                    SpawnParams.Instigator = GetInstigator();

                    // �߻�ü�� �����ϰ� �ʱ� ź���� ����
                    AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                    if (Projectile)
                    {
                        // �߻�ü�� �ʱ� ź���� �����մϴ�.
                        Projectile->FireInDirection(LaunchDirection);
                    }
                }
            }
        }
    }
}