#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"   // Enhanced Input�� ���� Subsystem ���
#include "EnhancedInputComponent.h"    // Enhanced Input Component ���
#include "FPSCharacter.h"    // �߰�

AFPSPlayerController::AFPSPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    CrouchAction(nullptr),
    Viewpoint_TransformationAction(nullptr) // �߰�
{
}

void AFPSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (InputMappingContext)
            {
                // Enhanced Input Subsystem�� Input Mapping Context �߰�
                Subsystem->AddMappingContext(InputMappingContext, 0);
               // UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context �߰� �Ϸ�!"));  // ������
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext�� �������� �ʾҽ��ϴ�!"));
            }
        }
    }
}
