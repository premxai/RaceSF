#include "SFVehiclePawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Materials/MaterialInterface.h"
#include "SFRouteRacer.h"
#include "UObject/ConstructorHelpers.h"

ASFVehiclePawn::ASFVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->InitBoxExtent(FVector(110.0f, 55.0f, 40.0f));
	CollisionBox->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionBox;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(CollisionBox);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeScale3D(FVector(2.2f, 1.1f, 0.8f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BodyMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (BodyMat.Succeeded())
	{
		BodyMesh->SetMaterial(0, BodyMat.Object);
	}

	ChaseSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseSpringArm"));
	ChaseSpringArm->SetupAttachment(RootComponent);
	ChaseSpringArm->TargetArmLength = 650.0f;
	ChaseSpringArm->SocketOffset = FVector(0.0f, 0.0f, 180.0f);
	ChaseSpringArm->bDoCollisionTest = false;
	ChaseSpringArm->bInheritPitch = false;
	ChaseSpringArm->bInheritRoll = false;
	ChaseSpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

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
	EnsureRuntimeInput();
	ApplyMappingContext();
}

void ASFVehiclePawn::EnsureRuntimeInput()
{
	if (!ThrottleAction)
	{
		ThrottleAction = NewObject<UInputAction>(this, TEXT("IA_Throttle"), RF_Transient);
		ThrottleAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!SteerAction)
	{
		SteerAction = NewObject<UInputAction>(this, TEXT("IA_Steer"), RF_Transient);
		SteerAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!BrakeAction)
	{
		BrakeAction = NewObject<UInputAction>(this, TEXT("IA_Brake"), RF_Transient);
		BrakeAction->ValueType = EInputActionValueType::Axis1D;
	}
	if (!HandbrakeAction)
	{
		HandbrakeAction = NewObject<UInputAction>(this, TEXT("IA_Handbrake"), RF_Transient);
		HandbrakeAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!ResetAction)
	{
		ResetAction = NewObject<UInputAction>(this, TEXT("IA_Reset"), RF_Transient);
		ResetAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!CameraAction)
	{
		CameraAction = NewObject<UInputAction>(this, TEXT("IA_Camera"), RF_Transient);
		CameraAction->ValueType = EInputActionValueType::Boolean;
	}
	if (!LookAction)
	{
		LookAction = NewObject<UInputAction>(this, TEXT("IA_Look"), RF_Transient);
		LookAction->ValueType = EInputActionValueType::Axis2D;
	}

	if (VehicleMappingContext)
	{
		return;
	}

	VehicleMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Vehicle"), RF_Transient);
	VehicleMappingContext->MapKey(ThrottleAction, EKeys::W);
	VehicleMappingContext->MapKey(ThrottleAction, EKeys::Up);
	VehicleMappingContext->MapKey(BrakeAction, EKeys::S);
	VehicleMappingContext->MapKey(BrakeAction, EKeys::Down);

	{
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(VehicleMappingContext);
		FEnhancedActionKeyMapping& AMap = VehicleMappingContext->MapKey(SteerAction, EKeys::A);
		AMap.Modifiers.Add(Negate);
	}
	VehicleMappingContext->MapKey(SteerAction, EKeys::D);

	VehicleMappingContext->MapKey(HandbrakeAction, EKeys::SpaceBar);
	VehicleMappingContext->MapKey(ResetAction, EKeys::R);
	VehicleMappingContext->MapKey(CameraAction, EKeys::C);
	VehicleMappingContext->MapKey(LookAction, EKeys::Mouse2D);
}

void ASFVehiclePawn::ApplyMappingContext()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController || !VehicleMappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(VehicleMappingContext, 0);
	}
}

void ASFVehiclePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnsureRuntimeInput();
	ApplyMappingContext();
}

void ASFVehiclePawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	ApplyMappingContext();
}

void ASFVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	EnsureRuntimeInput();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::ThrottleInput);
		EnhancedInput->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::ThrottleInput);
		EnhancedInput->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::SteeringInput);
		EnhancedInput->BindAction(SteerAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::SteeringInput);
		EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::BrakeInput);
		EnhancedInput->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::BrakeInput);
		EnhancedInput->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ASFVehiclePawn::HandbrakePressed);
		EnhancedInput->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ASFVehiclePawn::HandbrakeReleased);
		EnhancedInput->BindAction(ResetAction, ETriggerEvent::Started, this, &ASFVehiclePawn::ResetVehicle);
		EnhancedInput->BindAction(CameraAction, ETriggerEvent::Started, this, &ASFVehiclePawn::ToggleCamera);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASFVehiclePawn::LookInput);
	}
}

void ASFVehiclePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float AbsSpeed = FMath::Abs(CurrentSpeedCmPerSec);
	if (BrakeAxis > KINDA_SMALL_NUMBER || bHandbrake)
	{
		const float BrakeStrength = bHandbrake ? BrakeDecelCmPerSec2 * 1.35f : BrakeDecelCmPerSec2 * BrakeAxis;
		CurrentSpeedCmPerSec = FMath::FInterpTo(CurrentSpeedCmPerSec, 0.0f, DeltaSeconds, BrakeStrength / FMath::Max(1.0f, MaxSpeedCmPerSec));
	}
	else if (FMath::Abs(ThrottleAxis) > KINDA_SMALL_NUMBER)
	{
		CurrentSpeedCmPerSec += ThrottleAxis * AccelerationCmPerSec2 * DeltaSeconds;
	}
	else
	{
		CurrentSpeedCmPerSec = FMath::FInterpTo(CurrentSpeedCmPerSec, 0.0f, DeltaSeconds, CoastDecelCmPerSec2 / FMath::Max(1.0f, MaxSpeedCmPerSec));
	}

	CurrentSpeedCmPerSec = FMath::Clamp(CurrentSpeedCmPerSec, -MaxSpeedCmPerSec * 0.35f, MaxSpeedCmPerSec);

	const float SpeedAlpha = FMath::Clamp(AbsSpeed / FMath::Max(1.0f, MaxSpeedCmPerSec), 0.0f, 1.0f);
	const float TurnScale = FMath::Lerp(0.35f, 1.0f, SpeedAlpha) * (bHandbrake ? HandbrakeTurnMultiplier : 1.0f);
	if (FMath::Abs(SteerAxis) > KINDA_SMALL_NUMBER && AbsSpeed > 5.0f)
	{
		const float YawDelta = SteerAxis * TurnRateDegreesPerSec * TurnScale * DeltaSeconds * FMath::Sign(CurrentSpeedCmPerSec);
		AddActorWorldRotation(FRotator(0.0f, YawDelta, 0.0f));
	}

	FVector Location = GetActorLocation();
	Location += GetActorForwardVector() * (CurrentSpeedCmPerSec * DeltaSeconds);

	const FVector TraceStart = Location + FVector(0.0f, 0.0f, GroundProbeHeightCm);
	const FVector TraceEnd = Location - FVector(0.0f, 0.0f, GroundProbeDepthCm);
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SFVehicleGround), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params)
		|| GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		Location.Z = Hit.ImpactPoint.Z + RideHeightCm;
	}

	FHitResult SweepHit;
	SetActorLocation(Location, true, &SweepHit, ETeleportType::None);
	ResetIfFallenOutOfBounds();
}

void ASFVehiclePawn::ThrottleInput(const FInputActionValue& Value)
{
	ThrottleAxis = Value.Get<float>();
}

void ASFVehiclePawn::SteeringInput(const FInputActionValue& Value)
{
	SteerAxis = Value.Get<float>();
}

void ASFVehiclePawn::BrakeInput(const FInputActionValue& Value)
{
	BrakeAxis = Value.Get<float>();
}

void ASFVehiclePawn::HandbrakePressed()
{
	bHandbrake = true;
}

void ASFVehiclePawn::HandbrakeReleased()
{
	bHandbrake = false;
}

void ASFVehiclePawn::LookInput(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (ChaseSpringArm)
	{
		ChaseSpringArm->AddRelativeRotation(FRotator(-Axis.Y, Axis.X, 0.0f));
	}
}

void ASFVehiclePawn::CaptureSpawnTransform()
{
	SpawnTransform = GetActorTransform();
	CurrentSpeedCmPerSec = 0.0f;
}

void ASFVehiclePawn::ResetVehicle()
{
	SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	CurrentSpeedCmPerSec = 0.0f;
	ThrottleAxis = 0.0f;
	SteerAxis = 0.0f;
	BrakeAxis = 0.0f;
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
	return FMath::Abs(CurrentSpeedCmPerSec) * 0.036f;
}
