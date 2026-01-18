// Projectile.cpp
#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetNotifyRigidBodyCollision(true);
	ProjectileMesh->SetGenerateOverlapEvents(false);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1400.0f;
	MovementComp->MaxSpeed = 1400.0f;
	MovementComp->ProjectileGravityScale = 0.0f;

	TrailParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(RootComponent);

	InitialLifeSpan = 1.0f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SpawnLocation = GetActorLocation();

	// Clamp lifespan to max travel distance
	if (MaxTravelDistance > 0.0f && MovementComp)
	{
		const float Speed = FMath::Max(MovementComp->InitialSpeed, 1.0f);
		const float RangeLife = FMath::Clamp(MaxTravelDistance / Speed, 0.05f, 30.0f);
		SetLifeSpan(FMath::Min(GetLifeSpan(), RangeLife));
	}

	if (ProjectileMesh)
	{
		ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}

	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const AActor* MyOwner = GetOwner();

	// Valid hit on another actor
	if (OtherActor && OtherActor != MyOwner && OtherActor != this)
	{
		if (MyOwner)
		{
			UGameplayStatics::ApplyDamage(
				OtherActor,
				Damage,
				MyOwner->GetInstigatorController(),
				this,
				UDamageType::StaticClass()
			);
		}

		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this, ImpactEffect, GetActorLocation(), GetActorRotation()
			);
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
		}

		if (HitCameraShakeClass)
		{
			if (const UWorld* World = GetWorld())
			{
				if (APlayerController* PC = World->GetFirstPlayerController())
				{
					const bool bPlayerHit = (OtherActor == PC->GetPawn());
					const bool bPlayerFired = (MyOwner && MyOwner->GetInstigatorController() == PC);

					if (bPlayerHit || bPlayerFired)
					{
						PC->ClientStartCameraShake(HitCameraShakeClass);
					}
				}
			}
		}
	}

	Destroy();
}
