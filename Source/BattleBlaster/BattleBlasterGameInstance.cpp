// BattleBlasterGameInstance.cpp
#include "BattleBlasterGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UBattleBlasterGameInstance::ChangeLevel(int32 Index)
{
	if (Index < 1 || Index > LastLevelIndex) return;

	CurrentLevelIndex = Index;

	const FString LevelName = FString::Printf(TEXT("Level_%d"), CurrentLevelIndex);

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, *LevelName);
	}
}

void UBattleBlasterGameInstance::LoadNextLevel()
{
	if (CurrentLevelIndex < LastLevelIndex)
	{
		ChangeLevel(CurrentLevelIndex + 1);
	}
	else
	{
		RestartGame();
	}
}

void UBattleBlasterGameInstance::RestartCurrentLevel()
{
	ChangeLevel(CurrentLevelIndex);
}

void UBattleBlasterGameInstance::RestartGame()
{
	ChangeLevel(1);
}