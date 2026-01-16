// Tank.cpp
#include "Tank.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

ATank::ATank()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

void ATank::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (ULocalPlayer* LP = PlayerController->GetLocalPlayer())
		{
			if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	SetPlayerEnabled(false);
}

void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PlayerController) return;

	FHitResult Hit;
	if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit))
	{
		RotateTurret(Hit.ImpactPoint, DeltaTime);
	}
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATank::MoveInput);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATank::MoveInputCompleted);
		EIC->BindAction(TurnAction, ETriggerEvent::Triggered, this, &ATank::TurnInput);
		EIC->BindAction(FireAction, ETriggerEvent::Started, this, &ATank::Fire);
	}
}

void ATank::MoveInput(const FInputActionValue& Value)
{
	const float Input = Value.Get<float>();
	bIsMovingBackward = (Input < 0.0f);

	UWorld* World = GetWorld();
	if (!World) return;

	const float DT = World->GetDeltaSeconds();
	const FVector Delta = FVector(Speed * Input * DT, 0.0f, 0.0f);
	AddActorLocalOffset(Delta, true);
}

void ATank::MoveInputCompleted(const FInputActionValue& Value)
{
	bIsMovingBackward = false;
}

void ATank::TurnInput(const FInputActionValue& Value)
{
	UWorld* World = GetWorld();
	if (!World) return;

	const float Input = Value.Get<float>();
	const float DT = World->GetDeltaSeconds();

	// Reverse steering when moving backward
	const float Mult = bIsMovingBackward ? -1.0f : 1.0f;
	const FRotator Delta = FRotator(0.0f, TurnRate * Input * Mult * DT, 0.0f);
	AddActorLocalRotation(Delta, true);
}

void ATank::HandleDestruction()
{
	Super::HandleDestruction();

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetPlayerEnabled(false);
	bIsAlive = false;
}

void ATank::SetPlayerEnabled(bool bEnabled)
{
	if (!PlayerController) return;

	if (bEnabled)
	{
		EnableInput(PlayerController);

		FInputModeGameAndUI Mode;
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		Mode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(Mode);

		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}
	else
	{
		DisableInput(PlayerController);
		PlayerController->bShowMouseCursor = false;
	}
}
