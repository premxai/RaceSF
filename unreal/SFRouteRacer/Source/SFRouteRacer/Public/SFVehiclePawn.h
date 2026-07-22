#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "SFVehiclePawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UChaosVehicleMovementComponent;
class UInputAction;
class UInputMappingContext;

/**
 * Chaos vehicle foundation with chase camera and MVP controls.
 */
UCLASS()
class SFROUTERACER_API ASFVehiclePawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ASFVehiclePawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

		UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ResetVehicle();

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ResetIfFallenOutOfBounds(float MinZCm = -500.0f);

	UFUNCTION(BlueprintCallable, Category = "SF|Vehicle")
	void ToggleCamera();

	UFUNCTION(BlueprintPure, Category = "SF|Vehicle")
	float GetSpeedKmh() const;

protected:
	virtual void BeginPlay() override;

	void ThrottleInput(const struct FInputActionValue& Value);
	void SteeringInput(const struct FInputActionValue& Value);
	void BrakeInput(const struct FInputActionValue& Value);
	void HandbrakePressed();
	void HandbrakeReleased();
	void LookInput(const struct FInputActionValue& Value);

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

	bool bUsingChaseCamera = true;
	FTransform SpawnTransform;
};
