// BasePawn.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BasePawn.generated.h"

class UCapsuleComponent;
class UStaticMeshComponent;
class AProjectile;
class UNiagaraSystem;

/**
 * Base class for all tank-like pawns (player and AI).
 */
UCLASS()
class BATTLEBLASTER_API ABasePawn : public APawn
{
	GENERATED_BODY()

public:
	ABasePawn();

	void RotateTurret(const FVector& TargetLocation, float DeltaTime);
	void Fire();
	virtual void HandleDestruction();

	// Combat stats (set by GameMode)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float TurretRotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ProjectileDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireCooldown = 0.5f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DeathEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;

	UCapsuleComponent* GetCapsuleComponent() const { return CapsuleComp.Get(); }

private:
	float LastFireTime = -999.0f;
};
