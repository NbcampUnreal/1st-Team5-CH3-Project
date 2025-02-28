#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"   
#include "EnhancedInputComponent.h"    
#include "FPSCharacter.h" 
#include "Weapon.h" 

AFPSPlayerController::AFPSPlayerController()
    : InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    CrouchAction(nullptr),
    Viewpoint_TransformationAction(nullptr),
    SelectWeapon1Action(nullptr),
    SelectWeapon2Action(nullptr),
    FireAction(nullptr),
    ReloadAction(nullptr)
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
                Subsystem->AddMappingContext(InputMappingContext, 0);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("InputMappingContext"));
            }
        }
    }
}

