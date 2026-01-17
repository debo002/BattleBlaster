// Tower.cpp
#include "Tower.h"
#include "Tank.h"
#include "DrawDebugHelpers.h"

ATower::ATower()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATower::BeginPlay()
{
	Super::BeginPlay();

	bIsEnabled = false;

	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&ATower::TryFire,
		FireRate,
		true
	);
}

void ATower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only track player if enabled, tank exists, in range, and can see player
	if (bIsEnabled && Tank && IsInRange() && HasLineOfSight())
	{
		RotateTurret(Tank->GetActorLocation(), DeltaTime);
	}
}

void ATower::TryFire()
{
	if (!bIsEnabled) return;
	if (!Tank || !Tank->IsAlive()) return;
	if (!IsInRange()) return;
	if (!HasLineOfSight()) return; // Don't shoot through walls

	Fire();
}

bool ATower::IsInRange() const
{
	if (!Tank) return false;

	const float DistSq = FVector::DistSquared(GetActorLocation(), Tank->GetActorLocation());
	return DistSq <= FMath::Square(FireRange);
}

bool ATower::HasLineOfSight() const
{
	if (!Tank) return false;

	// Trace from turret to player tank
	FVector Start = GetActorLocation();
	Start.Z += 60.0f;  // Offset up from base
	
	FVector End = Tank->GetActorLocation();
	End.Z += 60.0f;  // Target player center

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Tank);

	// Use WorldStatic channel to detect walls/obstacles
	bool bHitSomething = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		Start,
		End,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		Params
	);

	// Clear line of sight if we didn't hit anything
	return !bHitSomething;
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

void ATower::SetTowerEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
}
