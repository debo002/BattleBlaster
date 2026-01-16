// MainMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class BATTLEBLASTER_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void OnPlayClicked();

	UFUNCTION()
	void OnQuitClicked();

	UPROPERTY(meta=(BindWidget))
	class UButton* PlayButton;

	UPROPERTY(meta=(BindWidget))
	class UButton* QuitButton;
};
