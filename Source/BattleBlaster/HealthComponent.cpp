// HealthComponent.cpp
#include "HealthComponent.h"
#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnDamageTaken);
	}

	if (UWorld* World = GetWorld())
	{
		GameModeRef = Cast<ABattleBlasterGameMode>(UGameplayStatics::GetGameMode(World));
	}
}

void UHealthComponent::SetMaxHealth(float NewMax)
{
	MaxHealth = NewMax;
	Health = MaxHealth;
}

void UHealthComponent::OnDamageTaken(AActor* DamagedActor, float DamageAmount, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	// Already dead or invalid damage
	if (Health <= 0.0f || DamageAmount <= 0.0f) return;

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

	if (Health <= 0.0f && GameModeRef)
	{
		GameModeRef->ActorDied(DamagedActor);
	}
}

