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

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1500.0f;
	MovementComp->MaxSpeed = 1500.0f;

	TrailParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailParticles"));
	TrailParticles->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}

	if (HitCameraShakeClass)
	{
		if (UWorld* World = GetWorld())
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				PC->ClientStartCameraShake(HitCameraShakeClass);
			}
		}
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const AActor* MyOwner = GetOwner();

	if (MyOwner && OtherActor && OtherActor != MyOwner && OtherActor != this)
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			MyOwner->GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

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
	}

	Destroy();
}
