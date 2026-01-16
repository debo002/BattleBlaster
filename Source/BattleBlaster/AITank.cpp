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
		UpdateAiming(DeltaTime);
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

	// Direction: toward player if too far, away if too close
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

	// Move with collision sweep
	const FVector Delta = Dir * MoveSpeed * DeltaTime;
	FHitResult Hit;
	AddActorWorldOffset(Delta, true, &Hit);

	// Rotate body toward movement
	const FRotator TargetRot = Dir.Rotation();
	const FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 2.0f);
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
