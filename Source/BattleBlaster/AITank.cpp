// AITank.cpp
#include "AITank.h"
#include "Tank.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAITank::AAITank()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAITank::BeginPlay()
{
	Super::BeginPlay();

	bIsEnabled = false;

	// Fire timer uses FireRate as interval
	GetWorldTimerManager().SetTimer(
		FireTimerHandle,
		this,
		&AAITank::TryFire,
		FireRate,
		true
	);
}

void AAITank::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAITank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsEnabled || !PlayerTank) return;

	UpdateBehavior(DeltaTime);
}

void AAITank::SetAIEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
	
	// Debug: Log when AI is enabled
	if (bEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Tank %s ENABLED"), *GetName());
	}
}

void AAITank::HandleDestruction()
{
	Super::HandleDestruction();

	bIsEnabled = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void AAITank::UpdateBehavior(float DeltaTime)
{
	if (!PlayerTank) return;

	const float Dist = GetDistanceToPlayer();
	bPlayerInSight = (Dist <= SightRange);

	if (bPlayerInSight)
	{
		// Only aim if we can actually see the player (no walls blocking)
		if (HasLineOfSight())
		{
			UpdateAiming(DeltaTime);
		}
		
		UpdateMovement(DeltaTime);
	}
}

void AAITank::UpdateMovement(float DeltaTime)
{
	if (!PlayerTank)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Tank %s: No PlayerTank reference!"), *GetName());
		return;
	}

	const float Dist = GetDistanceToPlayer();
	const float Error = Dist - PreferredDistance;

	// Within tolerance - hold position
	if (FMath::Abs(Error) <= DistanceTolerance)
	{
		return;
	}

	// Determine target location
	FVector TargetLocation;
	if (Error > 0.0f)
	{
		// Too far - move toward player
		TargetLocation = PlayerTank->GetActorLocation();
	}
	else
	{
		// Too close - move away from player (find position behind us)
		FVector AwayDir = (GetActorLocation() - PlayerTank->GetActorLocation()).GetSafeNormal();
		TargetLocation = GetActorLocation() + (AwayDir * 500.0f);
	}

	// Use Navigation System to find a path
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Tank %s: No Navigation System found!"), *GetName());
		return;
	}

	// Find a navigable path to target
	FNavLocation NavLocation;
	if (!NavSys->ProjectPointToNavigation(TargetLocation, NavLocation, FVector(500.0f, 500.0f, 500.0f)))
	{
		// Can't find valid navigation point near target
		UE_LOG(LogTemp, Warning, TEXT("AI Tank %s: Cannot find valid nav point near target!"), *GetName());
		return;
	}

	// Calculate path using navigation system
	FPathFindingQuery Query;
	Query.StartLocation = GetActorLocation();
	Query.EndLocation = NavLocation.Location;
	Query.NavData = NavSys->GetDefaultNavDataInstance();

	FPathFindingResult PathResult = NavSys->FindPathSync(Query);
	
	if (!PathResult.IsSuccessful() || !PathResult.Path.IsValid())
	{
		// No valid path found - try direct movement as fallback
		FVector Dir = (TargetLocation - GetActorLocation()).GetSafeNormal();
		Dir.Z = 0.0f;
		Dir.Normalize();

		const FVector Delta = Dir * MoveSpeed * DeltaTime;
		FHitResult Hit;
		AddActorWorldOffset(Delta, true, &Hit);

		// Rotate toward movement
		const FRotator TargetRot = Dir.Rotation();
		SetActorRotation(FRotator(0.0f, TargetRot.Yaw, 0.0f));
		return;
	}

	// Get next point in path
	const TArray<FNavPathPoint>& PathPoints = PathResult.Path->GetPathPoints();
	if (PathPoints.Num() < 2)
	{
		return;
	}

	// Move toward next waypoint in path (skip first point as it's our current location)
	FVector NextWaypoint = PathPoints[1].Location;
	FVector Dir = (NextWaypoint - GetActorLocation()).GetSafeNormal();
	Dir.Z = 0.0f;
	Dir.Normalize();

	// Apply movement
	const FVector Delta = Dir * MoveSpeed * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	// Rotate body toward movement direction smoothly
	const FRotator TargetRot = Dir.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 3.0f);
	SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
}

void AAITank::UpdateAiming(float DeltaTime)
{
	if (!PlayerTank) return;

	RotateTurret(PlayerTank->GetActorLocation(), DeltaTime);
}

void AAITank::TryFire()
{
	if (!bIsEnabled || !PlayerTank || !bPlayerInSight) return;
	if (!IsPlayerInRange(FireRange)) return;
	if (!IsFacingPlayer()) return;
	if (!HasLineOfSight()) return; // Don't shoot through walls

	Fire();
}

float AAITank::GetDistanceToPlayer() const
{
	if (!PlayerTank) return MAX_FLT;
	return FVector::Dist(GetActorLocation(), PlayerTank->GetActorLocation());
}

bool AAITank::IsPlayerInRange(float Range) const
{
	return GetDistanceToPlayer() <= Range;
}

bool AAITank::IsFacingPlayer() const
{
	if (!PlayerTank || !TurretMesh) return false;

	FVector ToPlayer = PlayerTank->GetActorLocation() - TurretMesh->GetComponentLocation();
	ToPlayer.Z = 0.0f;
	ToPlayer.Normalize();

	FVector Forward = TurretMesh->GetForwardVector();
	Forward.Z = 0.0f;
	Forward.Normalize();

	const float Dot = FVector::DotProduct(Forward, ToPlayer);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));

	return Angle <= AimAccuracyThreshold;
}

bool AAITank::HasLineOfSight() const
{
	if (!PlayerTank) return false;

	// Line trace from turret to player
	FVector Start = GetActorLocation();
	Start.Z += 50.0f; // Offset up from base
	FVector End = PlayerTank->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(PlayerTank);

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

