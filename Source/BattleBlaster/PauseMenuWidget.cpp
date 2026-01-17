// PauseMenuWidget.cpp
#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BattleBlasterGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

void UPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (RestartLevelButton)
	{
		RestartLevelButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartLevelClicked);
	}

	if (RestartGameButton)
	{
		RestartGameButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartGameClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UGameplayStatics::SetGamePaused(this, false);

		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(Mode);

		PC->bShowMouseCursor = true;
	}

	RemoveFromParent();
}

void UPauseMenuWidget::OnRestartLevelClicked()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		GI->RestartCurrentLevel();
	}
}

void UPauseMenuWidget::OnRestartGameClicked()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		GI->RestartGame();
	}
}

void UPauseMenuWidget::OnQuitClicked()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
