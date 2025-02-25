#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"   // Enhanced Input을 위한 Subsystem 헤더
#include "EnhancedInputComponent.h"    // Enhanced Input Component 헤더
#include "FPSCharacter.h"    // 추가

AFPSPlayerController::AFPSPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    CrouchAction(nullptr),
    Viewpoint_TransformationAction(nullptr) // 추가
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
                // Enhanced Input Subsystem에 Input Mapping Context 추가
                Subsystem->AddMappingContext(InputMappingContext, 0);
               // UE_LOG(LogTemp, Warning, TEXT("Input Mapping Context 추가 완료!"));  // 수정됨
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext가 설정되지 않았습니다!"));
            }
        }
    }
}
