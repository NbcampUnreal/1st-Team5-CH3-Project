#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "FPSPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
    GENERATED_BODY()

public: 

    AFPSPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* CrouchAction;

    virtual void BeginPlay() override;
};
