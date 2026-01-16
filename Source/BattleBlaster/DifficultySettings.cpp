// DifficultySettings.cpp
#include "DifficultySettings.h"

FDifficultySettings FDifficultyManager::EasySettings;
FDifficultySettings FDifficultyManager::MediumSettings;
FDifficultySettings FDifficultyManager::HardSettings;
bool FDifficultyManager::bInitialized = false;

void FDifficultyManager::InitializeSettings()
{
	if (bInitialized) return;

	// Level 1: Forgiving intro
	EasySettings.PlayerHealth = 200.0f;
	EasySettings.PlayerDamage = 50.0f;
	EasySettings.AITankHealth = 50.0f;
	EasySettings.AITankDamage = 10.0f;
	EasySettings.AITankFireRate = 4.0f;
	EasySettings.AITankMoveSpeed = 200.0f;
	EasySettings.AITankTurretRotationSpeed = 3.0f;
	EasySettings.AITankSightRange = 1200.0f;
	EasySettings.AITankFireRange = 800.0f;
	EasySettings.AITankAimAccuracy = 30.0f;
	EasySettings.TowerHealth = 50.0f;
	EasySettings.TowerDamage = 10.0f;
	EasySettings.TowerFireRate = 4.0f;
	EasySettings.TowerTurretRotationSpeed = 3.0f;
	EasySettings.TowerFireRange = 1000.0f;

	// Level 2: Balanced challenge
	MediumSettings.PlayerHealth = 100.0f;
	MediumSettings.PlayerDamage = 25.0f;
	MediumSettings.AITankHealth = 100.0f;
	MediumSettings.AITankDamage = 20.0f;
	MediumSettings.AITankFireRate = 2.5f;
	MediumSettings.AITankMoveSpeed = 350.0f;
	MediumSettings.AITankTurretRotationSpeed = 5.0f;
	MediumSettings.AITankSightRange = 2000.0f;
	MediumSettings.AITankFireRange = 1200.0f;
	MediumSettings.AITankAimAccuracy = 15.0f;
	MediumSettings.TowerHealth = 100.0f;
	MediumSettings.TowerDamage = 20.0f;
	MediumSettings.TowerFireRate = 2.5f;
	MediumSettings.TowerTurretRotationSpeed = 5.0f;
	MediumSettings.TowerFireRange = 1400.0f;

	// Level 3: High lethality
	HardSettings.PlayerHealth = 60.0f;
	HardSettings.PlayerDamage = 20.0f;
	HardSettings.AITankHealth = 200.0f;
	HardSettings.AITankDamage = 30.0f;
	HardSettings.AITankFireRate = 1.5f;
	HardSettings.AITankMoveSpeed = 500.0f;
	HardSettings.AITankTurretRotationSpeed = 8.0f;
	HardSettings.AITankSightRange = 3000.0f;
	HardSettings.AITankFireRange = 1800.0f;
	HardSettings.AITankAimAccuracy = 5.0f;
	HardSettings.TowerHealth = 250.0f;
	HardSettings.TowerDamage = 35.0f;
	HardSettings.TowerFireRate = 1.5f;
	HardSettings.TowerTurretRotationSpeed = 8.0f;
	HardSettings.TowerFireRange = 2200.0f;

	bInitialized = true;
}

const FDifficultySettings& FDifficultyManager::GetSettingsForLevel(int32 Level)
{
	InitializeSettings();

	switch (Level)
	{
	case 1:  return EasySettings;
	case 2:  return MediumSettings;
	case 3:  return HardSettings;
	default: return MediumSettings;
	}
}
