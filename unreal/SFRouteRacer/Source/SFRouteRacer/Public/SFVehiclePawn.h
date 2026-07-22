#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SFVehiclePawn.generated.h"

class UBoxComponent;
class UCameraComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;

/**
 * Arcade graybox vehicle for MVP playability without Chaos Blueprint assets.
 * Runtime Enhanced Input is created if no IMC/actions are assigned.
 */
UCLASS()
class SFROUTERACER_API ASFVehiclePawn : public APawn
{
	GENERATED_BODY()

public:
	ASFVehiclePawn();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void NotifyControllerChanged() override;

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ResetVehicle();

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void CaptureSpawnTransform();

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ResetIfFallenOutOfBounds(float MinZCm = -500.0f);

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ToggleCamera();

	UFUNCTION(BlueprintPure, Category = "SF|Vehicle")
	float GetSpeedKmh() const;

protected:
	virtual void BeginPlay() override;

	void EnsureRuntimeInput();
	void ApplyMappingContext();

	void ThrottleInput(const struct FInputActionValue& Value);
	void SteeringInput(const struct FInputActionValue& Value);
	void BrakeInput(const struct FInputActionValue& Value);
	void HandbrakePressed();
	void HandbrakeReleased();
	void LookInput(const struct FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Vehicle")
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Vehicle")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Camera")
	TObjectPtr<USpringArmComponent> ChaseSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Camera")
	TObjectPtr<UCameraComponent> ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Camera")
	TObjectPtr<UCameraComponent> HoodCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputMappingContext> VehicleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> SteerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> BrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> HandbrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> ResetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> CameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float MaxSpeedCmPerSec = 2800.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float AccelerationCmPerSec2 = 1600.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float BrakeDecelCmPerSec2 = 3200.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float CoastDecelCmPerSec2 = 600.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float TurnRateDegreesPerSec = 95.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float HandbrakeTurnMultiplier = 1.45f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float GroundProbeHeightCm = 800.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float GroundProbeDepthCm = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Vehicle")
	float RideHeightCm = 55.0f;

	bool bUsingChaseCamera = true;
	bool bHandbrake = false;
	float ThrottleAxis = 0.0f;
	float SteerAxis = 0.0f;
	float BrakeAxis = 0.0f;
	float CurrentSpeedCmPerSec = 0.0f;
	FTransform SpawnTransform;
};
