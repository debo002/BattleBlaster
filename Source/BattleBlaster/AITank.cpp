// AITank.cpp
#include "AITank.h"
#include "Tank.h"

AAITank::AAITank()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAITank::BeginPlay()
{
	Super::BeginPlay();

	bIsEnabled = false;

	// Fire timer
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
		// Check if we can actually see the player (no walls)
		bCanSeePlayer = HasLineOfSight();
		
		// Only aim at player if we can see them
		if (bCanSeePlayer)
		{
			UpdateAiming(DeltaTime);
		}
		
		// Always try to move toward player (will find path around obstacles)
		UpdateMovement(DeltaTime);
	}
}

void AAITank::UpdateMovement(float DeltaTime)
{
	if (!PlayerTank) return;

	const float Dist = GetDistanceToPlayer();
	const float Error = Dist - PreferredDistance;

	// Within tolerance - hold position
	if (FMath::Abs(Error) <= DistanceTolerance)
	{
		return;
	}

	// Determine if we should move toward or away from player
	bool bMoveTowardPlayer = (Error > 0.0f);
	
	// Get direction to player
	FVector ToPlayer = (PlayerTank->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	ToPlayer.Z = 0.0f;
	ToPlayer.Normalize();

	// Calculate desired movement direction
	FVector DesiredDir = bMoveTowardPlayer ? ToPlayer : -ToPlayer;

	// Check multiple distances ahead for obstacles
	FVector MoveDir = DesiredDir;
	
	// Check close range first (immediate obstacle)
	if (IsPathBlocked(DesiredDir, 100.0f))
	{
		MoveDir = FindUnblockedDirection(DesiredDir);
	}
	// Then check medium range (upcoming obstacle)
	else if (IsPathBlocked(DesiredDir, 200.0f))
	{
		// Start turning early for smoother navigation
		FVector EarlyTurnDir = FindUnblockedDirection(DesiredDir);
		// Blend between desired and turn direction for smoother movement
		MoveDir = FMath::Lerp(DesiredDir, EarlyTurnDir, 0.5f).GetSafeNormal();
	}

	// Apply movement
	const FVector Delta = MoveDir * MoveSpeed * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	// If still hitting something, try perpendicular movement
	if (Hit.bBlockingHit)
	{
		// Slide along the wall
		FVector SlideDir = FVector::CrossProduct(Hit.Normal, FVector::UpVector);
		
		// Choose slide direction that moves us closer to target
		if (FVector::DotProduct(SlideDir, DesiredDir) < 0)
		{
			SlideDir = -SlideDir;
		}
		
		AddActorWorldOffset(SlideDir * MoveSpeed * 0.5f * DeltaTime, true);
	}

	// Rotate body toward movement direction
	if (!MoveDir.IsNearlyZero())
	{
		const FRotator TargetRot = MoveDir.Rotation();
		const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 3.0f);
		SetActorRotation(FRotator(0.0f, NewRot.Yaw, 0.0f));
	}
}

bool AAITank::IsPathBlocked(const FVector& Direction, float Distance) const
{
	FVector Start = GetActorLocation();
	Start.Z += 30.0f;
	FVector End = Start + (Direction * Distance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// Use WorldStatic to detect walls/blocks (same as line of sight)
	bool bHit = GetWorld()->LineTraceSingleByObjectType(
		Hit,
		Start,
		End,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		Params
	);

	return bHit;
}

FVector AAITank::FindUnblockedDirection(const FVector& DesiredDir) const
{
	// Try different angles to find unblocked path
	const float AngleStep = 15.0f;  // Smaller steps for better precision
	const float MaxAngle = 180.0f;  // Check full semicircle
	const float CheckDistance = 250.0f;  // Longer check distance

	// Try alternating left and right with increasing angles
	for (float Angle = AngleStep; Angle <= MaxAngle; Angle += AngleStep)
	{
		// Try right first
		FVector RightDir = DesiredDir.RotateAngleAxis(Angle, FVector::UpVector);
		if (!IsPathBlocked(RightDir, CheckDistance))
		{
			return RightDir;
		}

		// Try left
		FVector LeftDir = DesiredDir.RotateAngleAxis(-Angle, FVector::UpVector);
		if (!IsPathBlocked(LeftDir, CheckDistance))
		{
			return LeftDir;
		}
	}

	// All directions blocked - try moving perpendicular to wall
	FVector PerpendicularRight = FVector::CrossProduct(DesiredDir, FVector::UpVector);
	if (!IsPathBlocked(PerpendicularRight, CheckDistance))
	{
		return PerpendicularRight;
	}
	
	FVector PerpendicularLeft = -PerpendicularRight;
	if (!IsPathBlocked(PerpendicularLeft, CheckDistance))
	{
		return PerpendicularLeft;
	}

	// Completely stuck - return original direction
	return DesiredDir;
}

void AAITank::UpdateAiming(float DeltaTime)
{
	if (!PlayerTank) return;

	RotateTurret(PlayerTank->GetActorLocation(), DeltaTime);
}

void AAITank::TryFire()
{
	if (!bIsEnabled) return;
	if (!PlayerTank) return;
	if (!bPlayerInSight) return;
	if (!IsPlayerInRange(FireRange)) return;
	if (!HasLineOfSight()) return;  // CRITICAL: Don't shoot through walls
	if (!IsFacingPlayer()) return;

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
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

	return Angle <= AimAccuracyThreshold;
}

bool AAITank::HasLineOfSight() const
{
	if (!PlayerTank) return false;

	// Trace from our position to player
	FVector Start = GetActorLocation();
	Start.Z += 60.0f;  // Offset up from base
	
	FVector End = PlayerTank->GetActorLocation();
	End.Z += 60.0f;  // Target player center

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(PlayerTank);

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
