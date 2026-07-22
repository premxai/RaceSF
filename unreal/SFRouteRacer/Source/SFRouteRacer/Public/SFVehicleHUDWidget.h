#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFVehicleHUDWidget.generated.h"

UCLASS()
class SFROUTERACER_API USFVehicleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void UpdateSpeed(float SpeedKmh);

	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void UpdateGear(int32 Gear);

	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void UpdateRaceTimer(float ElapsedSeconds);

	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void UpdateDestinationDistance(float DistanceMeters);

	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void SetReroutingVisible(bool bVisible);
};
