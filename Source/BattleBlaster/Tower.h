// Tower.h
#pragma once

#include "CoreMinimal.h"
#include "BasePawn.h"
#include "Tower.generated.h"

class ATank;

/**
 * Stationary AI turret.
 */
UCLASS()
class BATTLEBLASTER_API ATower : public ABasePawn
{
	GENERATED_BODY()

public:
	ATower();

	virtual void Tick(float DeltaTime) override;
	virtual void HandleDestruction() override;

	void SetTowerEnabled(bool bEnabled);
	void ResetFireTimer();
	void SetPlayerTarget(ATank* InTarget);

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 2.5f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TWeakObjectPtr<ATank> PlayerTankWeak;
	
	bool bIsEnabled = false;
	FTimerHandle FireTimerHandle;

	void TryFire();
	bool IsInRange() const;
	bool HasLineOfSight() const;
	
	ATank* GetPlayerTank() const;
};
