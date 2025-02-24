#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "FPSHUD.generated.h"

/**
 *
 */
UCLASS()
class FPSPROJECT_API AFPSHUD : public AHUD
{
    GENERATED_BODY()

protected:
    // ȭ�� �߾ӿ� �׷����ϴ�.
    UPROPERTY(EditDefaultsOnly)
    UTexture2D* CrosshairTexture;

public:
    // HUD�� �⺻ ��� ���Դϴ�.
    virtual void DrawHUD() override;
};