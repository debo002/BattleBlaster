// AITank.cpp
#include "AITank.h"
#include "Tank.h"
#include "AIController.h"
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
	if (!PlayerTank) return;

	const float Dist = GetDistanceToPlayer();
	const float Error = Dist - PreferredDistance;

	// Within tolerance - hold position
	if (FMath::Abs(Error) <= DistanceTolerance) return;

	// Try AI pathfinding first
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		// Determine target location based on distance
		FVector TargetLocation;
		if (Error > 0.0f)
		{
			// Too far - move toward player
			TargetLocation = PlayerTank->GetActorLocation();
		}
		else
		{
			// Too close - move away from player
			FVector AwayDir = (GetActorLocation() - PlayerTank->GetActorLocation()).GetSafeNormal();
			TargetLocation = GetActorLocation() + (AwayDir * 500.0f);
		}

		// Use AI pathfinding with acceptance radius
		AIController->MoveToLocation(TargetLocation, DistanceTolerance, true, true, false, true);
	}
	else
	{
		// Fallback: Direct movement if no AI controller
		FVector Dir;
		if (Error > 0.0f)
		{
			Dir = (PlayerTank->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
		else
		{
			Dir = (GetActorLocation() - PlayerTank->GetActorLocation()).GetSafeNormal();
		}
		Dir.Z = 0.0f;
		Dir.Normalize();

		// Move with collision
		const FVector Delta = Dir * MoveSpeed * DeltaTime;
		FHitResult Hit;
		AddActorWorldOffset(Delta, true, &Hit);

		// Rotate body toward movement
		const FRotator TargetRot = Dir.Rotation();
		const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 2.0f);
		SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
	}
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

