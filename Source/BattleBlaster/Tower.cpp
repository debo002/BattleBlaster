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

	// Line trace from turret to player tank
	FVector Start = GetActorLocation();
	Start.Z += 50.0f; // Offset up from base
	FVector End = Tank->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Tank);

	// Trace for blocking objects (walls, obstacles)
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	// If nothing hit, we have clear line of sight
	return !Hit.bBlockingHit;
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
