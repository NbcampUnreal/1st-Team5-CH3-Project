#include "TestGameMode.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h" // PlayerController 클래스를 사용

ATestGameMode::ATestGameMode()
{
	DefaultPawnClass = AFPSCharacter::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
}