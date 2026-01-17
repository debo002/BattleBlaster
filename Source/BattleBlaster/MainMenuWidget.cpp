// MainMenuWidget.cpp
#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BattleBlasterGameInstance.h"

void UMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}
}

void UMainMenuWidget::OnPlayClicked()
{
	if (auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		GI->RestartGame();
	}
	else
	{
		UGameplayStatics::OpenLevel(this, TEXT("Level_1"));
	}
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}
