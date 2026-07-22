#pragma once

#include "CoreMinimal.h"
#include "SFMapTypes.h"

struct FSFMapLoadResult
{
	bool bSuccess = false;
	FString ErrorMessage;
	FSFMapManifestData Manifest;
	TArray<FSFRoadNodeData> Nodes;
	TArray<FSFRoadEdgeData> Edges;
	TArray<FSFLandmarkData> Landmarks;
	TArray<FSFRaceDefinitionData> Races;
	TArray<FSFMapTileData> Tiles;
};

/**
 * Loads and validates versioned SF MVP JSON exports. Fails closed on corrupt data.
 */
class SFGEORUNTIME_API FSFMapJsonLoader
{
public:
	static constexpr TCHAR RequiredSchemaVersion[] = TEXT("0.1.0");

	static bool LoadDirectory(const FString& ExportDirectory, FSFMapLoadResult& OutResult);
	static bool LoadManifest(const FString& ManifestPath, FSFMapManifestData& OutManifest, FString& OutError);
	static bool LoadGraph(const FString& GraphPath, TArray<FSFRoadNodeData>& OutNodes, TArray<FSFRoadEdgeData>& OutEdges, FString& OutError);
	static bool LoadLandmarks(const FString& Path, TArray<FSFLandmarkData>& OutLandmarks, FString& OutError);
	static bool LoadRaces(const FString& Path, TArray<FSFRaceDefinitionData>& OutRaces, FString& OutError);
	static bool LoadTile(const FString& Path, FSFMapTileData& OutTile, FString& OutError);
	static bool ValidateLoadedData(const FSFMapLoadResult& Result, FString& OutError);

private:
	static bool ReadJsonFile(const FString& Path, TSharedPtr<FJsonObject>& OutObject, FString& OutError);
	static bool ReadJsonArrayFile(const FString& Path, TArray<TSharedPtr<FJsonValue>>& OutArray, FString& OutError);
};
