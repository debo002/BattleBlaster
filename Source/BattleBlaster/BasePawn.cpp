// BasePawn.cpp
#include "BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Projectile.h"

ABasePawn::ABasePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

void ABasePawn::RotateTurret(const FVector& TargetLocation, float DeltaTime)
{
	if (!TurretMesh) return;

	const FVector ToTarget = TargetLocation - TurretMesh->GetComponentLocation();
	const FRotator TargetRot = FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f);
	const FRotator NewRot = FMath::RInterpTo(
		TurretMesh->GetComponentRotation(),
		TargetRot,
		DeltaTime,
		TurretRotationSpeed
	);
	TurretMesh->SetWorldRotation(NewRot);
}

void ABasePawn::Fire()
{
	if (!ProjectileClass || !ProjectileSpawnPoint) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const float Now = World->GetTimeSeconds();
	if (Now - LastFireTime < FireCooldown) return;
	LastFireTime = Now;

	AProjectile* Proj = World->SpawnActor<AProjectile>(
		ProjectileClass,
		ProjectileSpawnPoint->GetComponentLocation(),
		ProjectileSpawnPoint->GetComponentRotation()
	);

	if (Proj)
	{
		Proj->SetOwner(this);
		Proj->Damage = ProjectileDamage;
	}
}

void ABasePawn::HandleDestruction()
{
	if (CapsuleComp) CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (BaseMesh) BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (TurretMesh) TurretMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DeathEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, DeathEffect, GetActorLocation(), GetActorRotation()
		);
	}

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathCameraShakeClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				PC->ClientStartCameraShake(DeathCameraShakeClass);
			}
		}
	}
}
