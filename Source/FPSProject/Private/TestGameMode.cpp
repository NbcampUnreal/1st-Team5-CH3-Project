#include "TestGameMode.h"
#include "FPSCharacter.h"
#include "FPSPlayerController.h" // PlayerController Ŭ������ ���

ATestGameMode::ATestGameMode()
{
	DefaultPawnClass = AFPSCharacter::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
}