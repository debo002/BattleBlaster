// PauseMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

/**
 * Pause menu UI with game control buttons.
 */
UCLASS()
class BATTLEBLASTER_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Button references bound from Blueprint
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartLevelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnRestartLevelClicked();

	UFUNCTION()
	void OnRestartGameClicked();

	UFUNCTION()
	void OnQuitClicked();
};
