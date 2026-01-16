// MainMenuPlayerController.cpp
#include "MainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!MainMenuWidgetClass) return;

	UUserWidget* Widget = CreateWidget(this, MainMenuWidgetClass);
	if (!Widget) return;

	Widget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Widget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
