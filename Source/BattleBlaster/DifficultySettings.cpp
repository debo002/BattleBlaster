// DifficultySettings.cpp
#include "DifficultySettings.h"

FDifficultySettings FDifficultyManager::EasySettings;
FDifficultySettings FDifficultyManager::MediumSettings;
FDifficultySettings FDifficultyManager::HardSettings;
bool FDifficultyManager::bInitialized = false;

void FDifficultyManager::InitializeSettings()
{
	if (bInitialized) return;

	// Level 1: Small enclosed map, close-quarters combat
	EasySettings.PlayerHealth = 200.0f;
	EasySettings.PlayerDamage = 50.0f;
	
	EasySettings.AITankHealth = 100.0f;
	EasySettings.AITankDamage = 50.0f;
	EasySettings.AITankFireRate = 2.5f;
	EasySettings.AITankMoveSpeed = 180.0f;
	EasySettings.AITankTurretRotationSpeed = 2.5f;
	EasySettings.AITankSightRange = 700.0f;
	EasySettings.AITankFireRange = 650.0f;
	EasySettings.AITankAimAccuracy = 25.0f;
	EasySettings.AITankPreferredDistance = 400.0f;
	EasySettings.AITankDistanceTolerance = 100.0f;
	
	EasySettings.TowerHealth = 100.0f;
	EasySettings.TowerDamage = 25.0f;
	EasySettings.TowerFireRate = 2.0f;
	EasySettings.TowerTurretRotationSpeed = 2.0f;
	EasySettings.TowerFireRange = 650.0f;
	
	EasySettings.ProjectileSpeed = 650.0f;
	EasySettings.ProjectileLifespan = 1.2f;
	EasySettings.ProjectileGravityScale = 0.5f;

	// Level 2: Open terrain, long-range engagements
	MediumSettings.PlayerHealth = 150.0f;
	MediumSettings.PlayerDamage = 50.0f;
	
	MediumSettings.AITankHealth = 150.0f;
	MediumSettings.AITankDamage = 40.0f;
	MediumSettings.AITankFireRate = 2.5f;
	MediumSettings.AITankMoveSpeed = 120.0f;
	MediumSettings.AITankTurretRotationSpeed = 2.5f;
	MediumSettings.AITankSightRange = 2500.0f;
	MediumSettings.AITankFireRange = 2200.0f;
	MediumSettings.AITankAimAccuracy = 15.0f;
	MediumSettings.AITankPreferredDistance = 1200.0f;
	MediumSettings.AITankDistanceTolerance = 200.0f;
	
	MediumSettings.TowerHealth = 150.0f;
	MediumSettings.TowerDamage = 30.0f;
	MediumSettings.TowerFireRate = 2.5f;
	MediumSettings.TowerTurretRotationSpeed = 2.5f;
	MediumSettings.TowerFireRange = 2200.0f;
	
	MediumSettings.ProjectileSpeed = 1100.0f;
	MediumSettings.ProjectileLifespan = 2.2f;
	MediumSettings.ProjectileGravityScale = 0.0f;

	// Level 3: Aggressive AI, fast-paced combat
	HardSettings.PlayerHealth = 150.0f;
	HardSettings.PlayerDamage = 50.0f;

	HardSettings.AITankHealth = 150.0f;
	HardSettings.AITankDamage = 45.0f;
	HardSettings.AITankFireRate = 1.8f;
	HardSettings.AITankMoveSpeed = 250.0f;
	HardSettings.AITankTurretRotationSpeed = 4.0f;
	HardSettings.AITankSightRange = 1800.0f;
	HardSettings.AITankFireRange = 1600.0f;
	HardSettings.AITankAimAccuracy = 10.0f;
	HardSettings.AITankPreferredDistance = 600.0f;
	HardSettings.AITankDistanceTolerance = 100.0f;

	HardSettings.TowerHealth = 150.0f;
	HardSettings.TowerDamage = 35.0f;
	HardSettings.TowerFireRate = 1.5f;
	HardSettings.TowerTurretRotationSpeed = 4.0f;
	HardSettings.TowerFireRange = 1600.0f;

	HardSettings.ProjectileSpeed = 1300.0f;
	HardSettings.ProjectileLifespan = 1.3f;
	HardSettings.ProjectileGravityScale = 0.0f;

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
