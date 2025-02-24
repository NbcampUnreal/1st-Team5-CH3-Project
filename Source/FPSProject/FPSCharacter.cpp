#include "FPSCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// 디폴트값 설정
AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 일인칭 카메라 컴포넌트를 생성합니다.
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

// 게임 시작 또는 스폰 시 호출
void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();
    check(GEngine != nullptr);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
}

// 프레임마다 호출
void AFPSCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 함수 기능을 입력에 바인딩하기 위해 호출
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
        // 발사 위치 계산: 카메라의 위치와 회전
        FVector CameraLocation;
        FRotator CameraRotation;
        GetActorEyesViewPoint(CameraLocation, CameraRotation);

        // MuzzleOffset을 설정하여 카메라 앞에서 발사체를 발사
        MuzzleOffset.Set(100.0f, 0.0f, 0.0f);
        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

        // 발사체 발사 각도
        FRotator MuzzleRotation = CameraRotation;
        MuzzleRotation.Pitch += 10.0f;

        // 화면 중앙의 크로스헤어를 기준으로 발사
        APlayerController* PlayerController = Cast<APlayerController>(GetController());
        if (PlayerController)
        {
            FVector MouseWorldLocation, MouseWorldDirection;
            float ScreenCenterX = GEngine->GameViewport->Viewport->GetSizeXY().X * 0.5f;  // 화면 중앙 X좌표
            float ScreenCenterY = GEngine->GameViewport->Viewport->GetSizeXY().Y * 0.5f;  // 화면 중앙 Y좌표

            // 화면 중앙을 월드 좌표로 변환
            if (PlayerController->DeprojectScreenPositionToWorld(ScreenCenterX, ScreenCenterY, MouseWorldLocation, MouseWorldDirection))
            {
                // 월드 좌표의 방향 벡터를 가져와 발사체의 방향으로 사용
                FVector LaunchDirection = MouseWorldDirection;  // 화면의 중앙을 기준으로 월드에서 방향을 구합니다.

                // 발사체 스폰
                UWorld* World = GetWorld();
                if (World)
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = this;
                    SpawnParams.Instigator = GetInstigator();

                    // 발사체를 스폰하고 초기 탄도를 설정
                    AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
                    if (Projectile)
                    {
                        // 발사체의 초기 탄도를 설정합니다.
                        Projectile->FireInDirection(LaunchDirection);
                    }
                }
            }
        }
    }
}