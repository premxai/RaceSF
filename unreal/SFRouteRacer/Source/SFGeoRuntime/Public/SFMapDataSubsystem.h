#pragma once

#include "CoreMinimal.h"
#include "SFMapTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SFMapDataSubsystem.generated.h"

/**
 * Owns the loaded SF MVP map export for the current game instance.
 */
UCLASS()
class SFGEORUNTIME_API USFMapDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	bool LoadExportDirectory(const FString& ExportDirectory);

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	bool LoadDefaultExport();

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	bool IsMapLoaded() const { return bMapLoaded; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const FSFMapManifestData& GetManifest() const { return Manifest; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const TArray<FSFRoadEdgeData>& GetEdges() const { return Edges; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const TArray<FSFRoadNodeData>& GetNodes() const { return Nodes; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const TArray<FSFLandmarkData>& GetLandmarks() const { return Landmarks; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const TArray<FSFRaceDefinitionData>& GetRaces() const { return Races; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	const TArray<FSFMapTileData>& GetTiles() const { return Tiles; }

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	bool FindLandmark(const FString& LandmarkId, FSFLandmarkData& OutLandmark) const;

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	bool FindRace(const FString& RaceId, FSFRaceDefinitionData& OutRace) const;

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	bool FindEdge(const FString& EdgeId, FSFRoadEdgeData& OutEdge) const;

	UFUNCTION(BlueprintPure, Category = "SF|Map")
	FString GetLastError() const { return LastError; }

	static FString ResolveDefaultExportDirectory();

private:
	UPROPERTY()
	FSFMapManifestData Manifest;

	UPROPERTY()
	TArray<FSFRoadNodeData> Nodes;

	UPROPERTY()
	TArray<FSFRoadEdgeData> Edges;

	UPROPERTY()
	TArray<FSFLandmarkData> Landmarks;

	UPROPERTY()
	TArray<FSFRaceDefinitionData> Races;

	UPROPERTY()
	TArray<FSFMapTileData> Tiles;

	TMap<FString, int32> EdgeIndexById;
	TMap<FString, int32> LandmarkIndexById;
	TMap<FString, int32> RaceIndexById;

	bool bMapLoaded = false;
	FString LastError;
};
