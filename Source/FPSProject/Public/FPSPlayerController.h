#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UInputMappingContext; // IMC ���� ���� ����
class UInputAction; // IA ���� ���� ����

UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** ������ */
	AFPSPlayerController();

	/** �Է� ���� ���ؽ�Ʈ (IMC) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	/** ĳ���� �̵� (IA_Move) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	/** ���� (IA_Jump) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	/** ī�޶� ȸ�� (IA_Look) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	/** �޸��� (IA_Sprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;

	/** 1��Ī �� 3��Ī ��ȯ */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* Viewpoint_TransformationAction; 

	/** �ɱ� (IA_Crouch) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

	/** ���� ���� �� ȣ��Ǵ� �Լ� */
	virtual void BeginPlay() override;

};
