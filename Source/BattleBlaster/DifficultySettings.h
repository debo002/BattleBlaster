// DifficultySettings.h
#pragma once

#include "CoreMinimal.h"
#include "DifficultySettings.generated.h"

USTRUCT(BlueprintType)
struct FDifficultySettings
{
	GENERATED_BODY()

	// Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float PlayerHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float PlayerDamage = 25.0f;

	// AI Tank
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankHealth = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankFireRate = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankMoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankTurretRotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankSightRange = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankFireRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankAimAccuracy = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankPreferredDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Tank")
	float AITankDistanceTolerance = 100.0f;

	// Tower
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TowerHealth = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TowerDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TowerFireRate = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TowerTurretRotationSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TowerFireRange = 900.0f;

	// Projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileLifespan = 3.0f;

	// 0 = no gravity, 1 = normal gravity
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileGravityScale = 0.0f;
};

/**
 * Static difficulty configuration manager.
 */
class BATTLEBLASTER_API FDifficultyManager
{
public:
	static const FDifficultySettings& GetSettingsForLevel(int32 Level);

private:
	static FDifficultySettings EasySettings;
	static FDifficultySettings MediumSettings;
	static FDifficultySettings HardSettings;
	static bool bInitialized;

	static void InitializeSettings();
};
