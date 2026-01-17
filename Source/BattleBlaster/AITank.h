// AITank.h
#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "AITank.generated.h"

class ATank;

/**
 * AI-controlled enemy tank.
 */
UCLASS()
class BATTLEBLASTER_API AAITank : public ABasePawn
{
	GENERATED_BODY()

public:
	AAITank();

	virtual void Tick(float DeltaTime) override;
	virtual void HandleDestruction() override;

	void SetAIEnabled(bool bEnabled);
	void ResetFireTimer();

	// Target reference (set by GameMode)
	UPROPERTY()
	TObjectPtr<ATank> PlayerTank;

	// Detection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Detection")
	float SightRange = 1500.0f;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float FireRange = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float FireRate = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float PreferredDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float DistanceTolerance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat")
	float AimAccuracyThreshold = 15.0f;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float MoveSpeed = 200.0f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool bIsEnabled = false;
	bool bPlayerInSight = false;
	bool bCanSeePlayer = false;  // True if line of sight is clear

	FTimerHandle FireTimerHandle;

	void UpdateBehavior(float DeltaTime);
	void UpdateMovement(float DeltaTime);
	void UpdateAiming(float DeltaTime);
	void TryFire();

	float GetDistanceToPlayer() const;
	bool IsPlayerInRange(float Range) const;
	bool IsFacingPlayer() const;
	bool HasLineOfSight() const;
	
	// Obstacle avoidance
	bool IsPathBlocked(const FVector& Direction, float Distance) const;
	FVector FindUnblockedDirection(const FVector& DesiredDir) const;
};
