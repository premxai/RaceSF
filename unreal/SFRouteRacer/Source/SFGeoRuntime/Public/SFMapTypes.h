#pragma once

#include "CoreMinimal.h"
#include "SFMapTypes.generated.h"

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFGeoPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double Latitude = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double Longitude = 0.0;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFBoundingBox
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double West = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double South = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double East = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Geo")
	double North = 0.0;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFRoadNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	double XMeters = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	double YMeters = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	double ElevationMeters = 0.0;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFRoadEdgeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	FString FromNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	FString ToNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	TArray<FVector> PointsLocalMeters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	float LengthMeters = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	float TravelTimeSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	FString RoadClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	int32 LaneCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	float LaneWidthMeters = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	float TotalWidthMeters = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	int32 SpeedLimitKph = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	bool bOneWay = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	bool bBridge = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	bool bTunnel = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	int32 Layer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Road")
	bool bDriveable = true;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFBuildingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Building")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Building")
	TArray<FVector2D> OuterRingLocalMeters;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Building")
	float HeightMeters = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Building")
	FString Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Building")
	FString TileId;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFSpawnPointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	FString EdgeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	FString NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	double XMeters = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	double YMeters = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	float HeadingDegrees = 0.0f;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFLandmarkData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	double Latitude = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	double Longitude = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	FSFSpawnPointData Spawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Landmark")
	bool bHasSpawn = false;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFSuggestedRouteData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	FString Profile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	float DistanceMeters = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	float EstimatedTimeSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	float OverlapWithFastest = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	TArray<FString> EdgeIds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Route")
	TArray<FString> NodeIds;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFRaceDefinitionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString RaceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString StartLandmarkId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString DestinationLandmarkId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TArray<FSFSuggestedRouteData> Routes;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFMapTileData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Tile")
	FString TileId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Tile")
	TArray<FString> RoadIds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Tile")
	TArray<FSFBuildingData> Buildings;
};

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFMapManifestData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FString SchemaVersion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FString Projection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FSFGeoPoint WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FSFBoundingBox BBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 TileSizeMeters = 250;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 TileCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 RoadCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 BuildingCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 LandmarkCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	int32 RaceCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FString GraphFile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FString LandmarksFile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	FString RacesFile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	TArray<FString> TileFiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	TArray<FString> Attribution;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Map")
	TMap<FString, FString> SourceDatasetVersions;
};
