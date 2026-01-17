// Tank.h
#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "InputActionValue.h"
#include "Tank.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

/**
 * Player-controlled tank.
 */
UCLASS()
class BATTLEBLASTER_API ATank : public ABasePawn
{
	GENERATED_BODY()

public:
	ATank();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void HandleDestruction() override;

	void SetPlayerEnabled(bool bEnabled);
	FORCEINLINE bool IsAlive() const { return bIsAlive; }

protected:
	virtual void BeginPlay() override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComp;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TurnAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	// UI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PauseMenuClass;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float Speed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float TurnRate = 80.0f;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	bool bIsAlive = true;
	bool bIsMovingBackward = false;

	void MoveInput(const FInputActionValue& Value);
	void MoveInputCompleted(const FInputActionValue& Value);
	void TurnInput(const FInputActionValue& Value);
	void TogglePause();
};
