#include "SFGeoCoordinateLibrary.h"

FVector USFGeoCoordinateLibrary::LocalMetersToUnreal(const FVector& LocalMeters)
{
	return FVector(LocalMeters.X * 100.0, -LocalMeters.Y * 100.0, LocalMeters.Z * 100.0);
}

FVector USFGeoCoordinateLibrary::UnrealToLocalMeters(const FVector& UnrealCentimeters)
{
	return FVector(UnrealCentimeters.X / 100.0, -UnrealCentimeters.Y / 100.0, UnrealCentimeters.Z / 100.0);
}

FVector USFGeoCoordinateLibrary::Point2DLocalToUnreal(double EastingMeters, double NorthingMeters, double ElevationMeters)
{
	return LocalMetersToUnreal(FVector(EastingMeters, NorthingMeters, ElevationMeters));
}
