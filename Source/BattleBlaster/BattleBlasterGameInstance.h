// BattleBlasterGameInstance.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BattleBlasterGameInstance.generated.h"

/**
 * Persistent game state across levels.
 */
UCLASS()
class BATTLEBLASTER_API UBattleBlasterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 LastLevelIndex = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex = 1;

	void LoadNextLevel();
	void RestartCurrentLevel();
	void RestartGame();

private:
	void ChangeLevel(int32 Index);
};
