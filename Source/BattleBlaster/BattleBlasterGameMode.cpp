// BattleBlasterGameMode.cpp
#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tank.h"
#include "Tower.h"
#include "AITank.h"
#include "HealthComponent.h"
#include "ScreenMessage.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	int32 LevelIndex = 1;
	FString MapNameStr = TEXT("Unknown");
	
	if (UWorld* World = GetWorld())
	{
		MapNameStr = World->GetMapName();
		MapNameStr.RemoveFromStart(World->StreamingLevelsPrefix);
		
		if (MapNameStr.Contains(TEXT("_3")) || MapNameStr.EndsWith(TEXT("3")))
		{
			LevelIndex = 3;
		}
		else if (MapNameStr.Contains(TEXT("_2")) || MapNameStr.EndsWith(TEXT("2")))
		{
			LevelIndex = 2;
		}
		else
		{
			LevelIndex = 1;
		}
	}
	
	// Update GameInstance to match detected level
	if (auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		GI->CurrentLevelIndex = LevelIndex;
	}
	
	CurrentSettings = FDifficultyManager::GetSettingsForLevel(LevelIndex);


	ApplyDifficultySettings();
	SetupUI();
	StartCountdown();
}

void ABattleBlasterGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	GetWorldTimerManager().ClearTimer(GameOverTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ABattleBlasterGameMode::ApplyDifficultySettings()
{
	// Player
	if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		PlayerTank = Cast<ATank>(Pawn);
		if (PlayerTank)
		{
			if (auto* HC = PlayerTank->FindComponentByClass<UHealthComponent>())
			{
				HC->SetMaxHealth(CurrentSettings.PlayerHealth);
			}
			PlayerTank->ProjectileDamage = CurrentSettings.PlayerDamage;
			PlayerTank->ProjectileSpeed = CurrentSettings.ProjectileSpeed;
			PlayerTank->ProjectileLifespan = CurrentSettings.ProjectileLifespan;
			PlayerTank->ProjectileGravityScale = CurrentSettings.ProjectileGravityScale;
		}
	}

	// Towers
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	TowerCount = Towers.Num();

	for (AActor* Actor : Towers)
	{
		if (ATower* Tower = Cast<ATower>(Actor))
		{
			Tower->SetPlayerTarget(PlayerTank);

			if (auto* HC = Tower->FindComponentByClass<UHealthComponent>())
			{
				HC->SetMaxHealth(CurrentSettings.TowerHealth);
			}
			Tower->ProjectileDamage = CurrentSettings.TowerDamage;
			Tower->FireRate = CurrentSettings.TowerFireRate;
			Tower->FireRange = CurrentSettings.TowerFireRange;
			Tower->TurretRotationSpeed = CurrentSettings.TowerTurretRotationSpeed;
			Tower->ProjectileSpeed = CurrentSettings.ProjectileSpeed;
			Tower->ProjectileLifespan = CurrentSettings.ProjectileLifespan;
			Tower->ProjectileGravityScale = CurrentSettings.ProjectileGravityScale;
			Tower->ResetFireTimer();
		}
	}

	// AI Tanks
	TArray<AActor*> AITanks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAITank::StaticClass(), AITanks);
	AITankCount = AITanks.Num();

	for (AActor* Actor : AITanks)
	{
		if (AAITank* AI = Cast<AAITank>(Actor))
		{
			AI->SetPlayerTarget(PlayerTank);

			if (auto* HC = AI->FindComponentByClass<UHealthComponent>())
			{
				HC->SetMaxHealth(CurrentSettings.AITankHealth);
			}
			AI->ProjectileDamage = CurrentSettings.AITankDamage;
			AI->FireRate = CurrentSettings.AITankFireRate;
			AI->FireRange = CurrentSettings.AITankFireRange;
			AI->MoveSpeed = CurrentSettings.AITankMoveSpeed;
			AI->SightRange = CurrentSettings.AITankSightRange;
			AI->AimAccuracyThreshold = CurrentSettings.AITankAimAccuracy;
			AI->TurretRotationSpeed = CurrentSettings.AITankTurretRotationSpeed;
			AI->PreferredDistance = CurrentSettings.AITankPreferredDistance;
			AI->DistanceTolerance = CurrentSettings.AITankDistanceTolerance;
			AI->ProjectileSpeed = CurrentSettings.ProjectileSpeed;
			AI->ProjectileLifespan = CurrentSettings.ProjectileLifespan;
			AI->ProjectileGravityScale = CurrentSettings.ProjectileGravityScale;
			AI->ResetFireTimer();
		}
	}
}

void ABattleBlasterGameMode::SetupUI()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !ScreenMessageClass) return;

	ScreenMessageWidget = CreateWidget<UScreenMessage>(PC, ScreenMessageClass);
	if (!ScreenMessageWidget) return;

	ScreenMessageWidget->AddToViewport(999);
	ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);

	if (auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance()))
	{
		ScreenMessageWidget->SetMessageText(FString::Printf(TEXT("Level %d"), GI->CurrentLevelIndex));
	}
	else
	{
		ScreenMessageWidget->SetMessageText(TEXT("Get Ready!"));
	}
}

void ABattleBlasterGameMode::StartCountdown()
{
	CountdownRemaining = CountdownDuration;
	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle,
		this,
		&ABattleBlasterGameMode::OnCountdownTick,
		1.0f,
		true
	);
}

void ABattleBlasterGameMode::OnCountdownTick()
{
	CountdownRemaining--;

	if (CountdownRemaining > 0)
	{
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetMessageText(FString::FromInt(CountdownRemaining));
		}
	}
	else if (CountdownRemaining == 0)
	{
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetMessageText(TEXT("Go!"));
		}

		if (PlayerTank)
		{
			PlayerTank->SetPlayerEnabled(true);
		}

		EnableAllAI();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABattleBlasterGameMode::EnableAllAI()
{
	TArray<AActor*> Towers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Towers);
	for (AActor* Actor : Towers)
	{
		if (ATower* Tower = Cast<ATower>(Actor))
		{
			Tower->SetTowerEnabled(true);
		}
	}

	TArray<AActor*> AITanks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAITank::StaticClass(), AITanks);
	for (AActor* Actor : AITanks)
	{
		if (AAITank* AI = Cast<AAITank>(Actor))
		{
			AI->SetAIEnabled(true);
		}
	}
}

void ABattleBlasterGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();
		bIsVictory = false;

		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetMessageText(TEXT("Defeat!"));
			ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		}

		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOver, GameOverDelay, false);
		return;
	}

	if (ATower* Tower = Cast<ATower>(DeadActor))
	{
		Tower->HandleDestruction();
		TowerCount--;
	}
	else if (AAITank* AI = Cast<AAITank>(DeadActor))
	{
		AI->HandleDestruction();
		AITankCount--;
	}

	// Victory check
	if (TowerCount <= 0 && AITankCount <= 0)
	{
		bIsVictory = true;

		if (ScreenMessageWidget)
		{
			ScreenMessageWidget->SetMessageText(TEXT("Victory!"));
			ScreenMessageWidget->SetVisibility(ESlateVisibility::Visible);
		}

		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ABattleBlasterGameMode::OnGameOver, GameOverDelay, false);
	}
}

void ABattleBlasterGameMode::OnGameOver()
{
	auto* GI = Cast<UBattleBlasterGameInstance>(GetGameInstance());
	if (!GI) return;

	if (bIsVictory)
	{
		GI->LoadNextLevel();
	}
	else
	{
		GI->RestartCurrentLevel();
	}
}
