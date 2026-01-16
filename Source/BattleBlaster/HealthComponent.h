// HealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABattleBlasterGameMode;

/**
 * Health component - attach to any damageable actor.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BATTLEBLASTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	FORCEINLINE bool IsAlive() const { return Health > 0.0f; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	void SetMaxHealth(float NewMax);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float Health = 0.0f;

private:
	UPROPERTY()
	TObjectPtr<ABattleBlasterGameMode> GameModeRef;

	UFUNCTION()
	void OnDamageTaken(AActor* DamagedActor, float DamageAmount, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
};
