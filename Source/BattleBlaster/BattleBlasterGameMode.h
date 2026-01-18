// BattleBlasterGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DifficultySettings.h"
#include "BattleBlasterGameMode.generated.h"

class ATank;
class ATower;
class AAITank;
class UScreenMessage;

/**
 * Main gameplay game mode.
 */
UCLASS()
class BATTLEBLASTER_API ABattleBlasterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	void ActorDied(AActor* DeadActor);

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UScreenMessage> ScreenMessageClass;

	UPROPERTY(EditAnywhere, Category = "Game")
	float GameOverDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Game")
	int32 CountdownDuration = 3;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<ATank> PlayerTank;

	UPROPERTY()
	TObjectPtr<UScreenMessage> ScreenMessageWidget;

	FDifficultySettings CurrentSettings;

	int32 TowerCount = 0;
	int32 AITankCount = 0;
	int32 CountdownRemaining = 0;
	bool bIsVictory = false;

	FTimerHandle CountdownTimerHandle;
	FTimerHandle GameOverTimerHandle;

	void ApplyDifficultySettings();
	void SetupUI();
	void StartCountdown();
	void OnCountdownTick();
	void EnableAllAI();
	void OnGameOver();
};
