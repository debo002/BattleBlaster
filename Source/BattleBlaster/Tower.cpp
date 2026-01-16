// Tower.cpp
#include "Tower.h"
#include "Tank.h"

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

	if (bIsEnabled && Tank && IsInRange())
	{
		RotateTurret(Tank->GetActorLocation(), DeltaTime);
	}
}

void ATower::TryFire()
{
	if (!bIsEnabled) return;
	if (!Tank || !Tank->IsAlive()) return;
	if (!IsInRange()) return;

	Fire();
}

bool ATower::IsInRange() const
{
	if (!Tank) return false;

	const float DistSq = FVector::DistSquared(GetActorLocation(), Tank->GetActorLocation());
	return DistSq <= FMath::Square(FireRange);
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
