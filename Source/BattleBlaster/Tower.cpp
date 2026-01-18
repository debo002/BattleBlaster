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

	ATank* Tank = GetPlayerTank();
	if (bIsEnabled && Tank && Tank->IsAlive() && IsInRange() && HasLineOfSight())
	{
		RotateTurret(Tank->GetActorLocation(), DeltaTime);
	}
}

void ATower::SetPlayerTarget(ATank* InTarget)
{
	PlayerTankWeak = InTarget;
}

ATank* ATower::GetPlayerTank() const
{
	return PlayerTankWeak.Get();
}

void ATower::TryFire()
{
	if (!bIsEnabled) return;
	
	ATank* Tank = GetPlayerTank();
	if (!Tank || !Tank->IsAlive()) return;
	if (!IsInRange()) return;
	if (!HasLineOfSight()) return;

	Fire();
}

bool ATower::IsInRange() const
{
	ATank* Tank = GetPlayerTank();
	if (!Tank) return false;

	const float DistSq = FVector::DistSquared(GetActorLocation(), Tank->GetActorLocation());
	return DistSq <= FMath::Square(FireRange);
}

bool ATower::HasLineOfSight() const
{
	ATank* Tank = GetPlayerTank();
	if (!Tank) return false;

	FVector Start = GetActorLocation();
	Start.Z += 60.0f;
	
	FVector End = Tank->GetActorLocation();
	End.Z += 60.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Tank);

	const bool bHitSomething = GetWorld()->LineTraceSingleByObjectType(
		Hit, Start, End,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		Params
	);

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

void ATower::ResetFireTimer()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATower::TryFire, FireRate, true);
}
