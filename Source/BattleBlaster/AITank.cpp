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
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAITank::TryFire, FireRate, true);
}

void AAITank::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void AAITank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ATank* PlayerTank = GetPlayerTank();
	if (!bIsEnabled || !PlayerTank || !PlayerTank->IsAlive()) return;
	
	UpdateBehavior(DeltaTime);
}

void AAITank::SetAIEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
}

void AAITank::SetPlayerTarget(ATank* InTarget)
{
	PlayerTankWeak = InTarget;
}

void AAITank::ResetFireTimer()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AAITank::TryFire, FireRate, true);
}

void AAITank::HandleDestruction()
{
	Super::HandleDestruction();
	bIsEnabled = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

ATank* AAITank::GetPlayerTank() const
{
	return PlayerTankWeak.Get();
}

void AAITank::UpdateBehavior(float DeltaTime)
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return;

	const float Dist = GetDistanceToPlayer();
	bPlayerInSight = (Dist <= SightRange);

	if (bPlayerInSight)
	{
		bCanSeePlayer = HasLineOfSight();
		
		if (bCanSeePlayer)
		{
			UpdateAiming(DeltaTime);
		}
		
		UpdateMovement(DeltaTime);
	}
}

void AAITank::UpdateMovement(float DeltaTime)
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return;

	const float Dist = GetDistanceToPlayer();
	const float Error = Dist - PreferredDistance;

	if (FMath::Abs(Error) <= DistanceTolerance) return;

	const bool bMoveTowardPlayer = (Error > 0.0f);
	
	FVector ToPlayer = PlayerTank->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.0f;
	ToPlayer = ToPlayer.GetSafeNormal();

	FVector DesiredDir = bMoveTowardPlayer ? ToPlayer : -ToPlayer;
	FVector MoveDir = DesiredDir;
	
	// Obstacle avoidance
	if (IsPathBlocked(DesiredDir, 100.0f))
	{
		MoveDir = FindUnblockedDirection(DesiredDir);
	}
	else if (IsPathBlocked(DesiredDir, 200.0f))
	{
		const FVector EarlyTurnDir = FindUnblockedDirection(DesiredDir);
		MoveDir = FMath::Lerp(DesiredDir, EarlyTurnDir, 0.5f).GetSafeNormal();
	}

	const FVector Delta = MoveDir * MoveSpeed * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	// Slide along walls
	if (Hit.bBlockingHit)
	{
		FVector SlideDir = FVector::CrossProduct(Hit.Normal, FVector::UpVector);
		if (FVector::DotProduct(SlideDir, DesiredDir) < 0.0f)
		{
			SlideDir = -SlideDir;
		}
		AddActorWorldOffset(SlideDir * MoveSpeed * 0.5f * DeltaTime, true);
	}

	// Face movement direction
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
	const FVector End = Start + (Direction * Distance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByObjectType(
		Hit, Start, End,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		Params
	);
}

FVector AAITank::FindUnblockedDirection(const FVector& DesiredDir) const
{
	constexpr float AngleStep = 15.0f;
	constexpr float MaxAngle = 180.0f;
	constexpr float CheckDistance = 250.0f;

	// Check alternating sides
	for (float Angle = AngleStep; Angle <= MaxAngle; Angle += AngleStep)
	{
		const FVector RightDir = DesiredDir.RotateAngleAxis(Angle, FVector::UpVector);
		if (!IsPathBlocked(RightDir, CheckDistance))
		{
			return RightDir;
		}

		const FVector LeftDir = DesiredDir.RotateAngleAxis(-Angle, FVector::UpVector);
		if (!IsPathBlocked(LeftDir, CheckDistance))
		{
			return LeftDir;
		}
	}

	// Fallback: perpendicular directions
	const FVector PerpendicularRight = FVector::CrossProduct(DesiredDir, FVector::UpVector);
	if (!IsPathBlocked(PerpendicularRight, CheckDistance))
	{
		return PerpendicularRight;
	}
	
	if (!IsPathBlocked(-PerpendicularRight, CheckDistance))
	{
		return -PerpendicularRight;
	}

	return DesiredDir;
}

void AAITank::UpdateAiming(float DeltaTime)
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return;
	
	RotateTurret(PlayerTank->GetActorLocation(), DeltaTime);
}

void AAITank::TryFire()
{
	if (!bIsEnabled) return;
	
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return;
	if (!bPlayerInSight) return;
	if (!IsPlayerInRange(FireRange)) return;
	if (!HasLineOfSight()) return;
	if (!IsFacingPlayer()) return;

	Fire();
}

float AAITank::GetDistanceToPlayer() const
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return MAX_FLT;
	
	return FVector::Dist(GetActorLocation(), PlayerTank->GetActorLocation());
}

bool AAITank::IsPlayerInRange(float Range) const
{
	return GetDistanceToPlayer() <= Range;
}

bool AAITank::IsFacingPlayer() const
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank || !TurretMesh) return false;

	const FVector ToPlayer = (PlayerTank->GetActorLocation() - TurretMesh->GetComponentLocation()).GetSafeNormal2D();
	const FVector Forward = TurretMesh->GetForwardVector().GetSafeNormal2D();
	const float Dot = FVector::DotProduct(Forward, ToPlayer);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

	return Angle <= AimAccuracyThreshold;
}

bool AAITank::HasLineOfSight() const
{
	ATank* PlayerTank = GetPlayerTank();
	if (!PlayerTank) return false;

	FVector Start = GetActorLocation();
	Start.Z += 60.0f;
	
	FVector End = PlayerTank->GetActorLocation();
	End.Z += 60.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(PlayerTank);

	const bool bHitSomething = GetWorld()->LineTraceSingleByObjectType(
		Hit, Start, End,
		FCollisionObjectQueryParams(ECC_WorldStatic),
		Params
	);

	return !bHitSomething;
}
