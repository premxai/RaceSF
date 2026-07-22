#include "SFVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "SFRouteRacer.h"

ASFVehiclePawn::ASFVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	ChaseSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseSpringArm"));
	ChaseSpringArm->SetupAttachment(RootComponent);
	ChaseSpringArm->TargetArmLength = 650.0f;
	ChaseSpringArm->SocketOffset = FVector(0.0f, 0.0f, 180.0f);
	ChaseSpringArm->bDoCollisionTest = true;
	ChaseSpringArm->bInheritPitch = false;
	ChaseSpringArm->bInheritRoll = false;

	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	ChaseCamera->SetupAttachment(ChaseSpringArm, USpringArmComponent::SocketName);
	ChaseCamera->bAutoActivate = true;

	HoodCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HoodCamera"));
	HoodCamera->SetupAttachment(RootComponent);
	HoodCamera->SetRelativeLocation(FVector(40.0f, 0.0f, 120.0f));
	HoodCamera->bAutoActivate = false;
}

void ASFVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	SpawnTransform = GetActorTransform();
}

void ASFVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (VehicleMappingContext)
			{
				Subsystem->AddMappingContext(VehicleMappingContext, 0);
			}
		}
	}
}

void ASFVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ThrottleAction)
		{
			EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::ThrottleInput);
			EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::ThrottleInput);
		}
		if (SteerAction)
		{
			EnhancedInput->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::SteeringInput);
			EnhancedInput->BindAction(SteerAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::SteeringInput);
		}
		if (BrakeAction)
		{
			EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::BrakeInput);
			EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::BrakeInput);
		}
		if (HandbrakeAction)
		{
			EnhancedInput->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ASFVehiclePawn::HandbrakePressed);
			EnhancedInput->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::HandbrakeReleased);
		}
		if (ResetAction)
		{
			EnhancedInput->BindAction(ResetAction, ETriggerEvent::Started, this, &ASFVehiclePawn::ResetVehicle);
		}
		if (CameraAction)
		{
			EnhancedInput->BindAction(CameraAction, ETriggerEvent::Started, this, &ASFVehiclePawn::ToggleCamera);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::LookInput);
		}
	}
}

void ASFVehiclePawn::ThrottleInput(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		Movement->SetThrottleInput(Value.Get<float>());
	}
}

void ASFVehiclePawn::SteeringInput(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		Movement->SetSteeringInput(Value.Get<float>());
	}
}

void ASFVehiclePawn::BrakeInput(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		Movement->SetBrakeInput(Value.Get<float>());
	}
}

void ASFVehiclePawn::HandbrakePressed()
{
	if (UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		Movement->SetHandbrakeInput(true);
	}
}

void ASFVehiclePawn::HandbrakeReleased()
{
	if (UChaosWheeledVehicleMovementComponent* Movement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement()))
	{
		Movement->SetHandbrakeInput(false);
	}
}

void ASFVehiclePawn::LookInput(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (ChaseSpringArm)
	{
		ChaseSpringArm->AddRelativeRotation(FRotator(-Axis.Y, Axis.X, 0.0f));
	}
}

void ASFVehiclePawn::ResetVehicle()
{
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	if (UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(RootComponent))
	{
		RootPrim->SetPhysicsLinearVelocity(FVector::ZeroVector);
		RootPrim->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
	UE_LOG(LogSFRace, Log, TEXT("Vehicle reset to spawn transform"));
}

void ASFVehiclePawn::ResetIfFallenOutOfBounds(float MinZCm)
{
	if (GetActorLocation().Z < MinZCm)
	{
		UE_LOG(LogSFRace, Warning, TEXT("Vehicle fell out of bounds; resetting"));
		ResetVehicle();
	}
}

void ASFVehiclePawn::ToggleCamera()
{
	bUsingChaseCamera = !bUsingChaseCamera;
	if (ChaseCamera)
	{
		ChaseCamera->SetActive(bUsingChaseCamera);
	}
	if (HoodCamera)
	{
		HoodCamera->SetActive(!bUsingChaseCamera);
	}
}

float ASFVehiclePawn::GetSpeedKmh() const
{
	const float CmPerSecond = GetVelocity().Size();
	return CmPerSecond * 0.036f;
}
