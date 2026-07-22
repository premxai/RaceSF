#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SFGeoCoordinateLibrary.generated.h"

/**
 * Single authoritative conversion between local meters and Unreal centimeters.
 * Geographic projection is performed offline in Python; runtime only remaps axes/units.
 */
UCLASS()
class SFGEORUNTIME_API USFGeoCoordinateLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Unreal X = easting_m * 100, Y = -northing_m * 100, Z = elevation_m * 100. */
	UFUNCTION(BlueprintPure, Category = "SF|Geo")
	static FVector LocalMetersToUnreal(const FVector& LocalMeters);

	UFUNCTION(BlueprintPure, Category = "SF|Geo")
	static FVector UnrealToLocalMeters(const FVector& UnrealCentimeters);

	UFUNCTION(BlueprintPure, Category = "SF|Geo")
	static FVector Point2DLocalToUnreal(double EastingMeters, double NorthingMeters, double ElevationMeters = 0.0);
};
