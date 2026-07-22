#include "SFMapJsonLoader.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "SFGeoRuntime.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace SFMapJsonPrivate
{
	bool RequireString(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, FString& OutValue, FString& OutError)
	{
		if (!Object.IsValid() || !Object->TryGetStringField(Field, OutValue))
		{
			OutError = FString::Printf(TEXT("Missing required string field '%s'"), Field);
			return false;
		}
		return true;
	}

	bool GetNumber(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, double& OutValue)
	{
		return Object.IsValid() && Object->TryGetNumberField(Field, OutValue);
	}
}

bool FSFMapJsonLoader::ReadJsonFile(const FString& Path, TSharedPtr<FJsonObject>& OutObject, FString& OutError)
{
	FString Raw;
	if (!FFileHelper::LoadFileToString(Raw, *Path))
	{
		OutError = FString::Printf(TEXT("Unable to read JSON file: %s"), *Path);
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Raw);
	if (!FJsonSerializer::Deserialize(Reader, OutObject) || !OutObject.IsValid())
	{
		OutError = FString::Printf(TEXT("Unable to parse JSON object: %s"), *Path);
		return false;
	}
	return true;
}

bool FSFMapJsonLoader::ReadJsonArrayFile(const FString& Path, TArray<TSharedPtr<FJsonValue>>& OutArray, FString& OutError)
{
	FString Raw;
	if (!FFileHelper::LoadFileToString(Raw, *Path))
	{
		OutError = FString::Printf(TEXT("Unable to read JSON file: %s"), *Path);
		return false;
	}

	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Raw);
	if (!FJsonSerializer::Deserialize(Reader, OutArray))
	{
		OutError = FString::Printf(TEXT("Unable to parse JSON array: %s"), *Path);
		return false;
	}
	return true;
}

bool FSFMapJsonLoader::LoadManifest(const FString& ManifestPath, FSFMapManifestData& OutManifest, FString& OutError)
{
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(ManifestPath, Root, OutError))
	{
		return false;
	}

	if (!SFMapJsonPrivate::RequireString(Root, TEXT("schema_version"), OutManifest.SchemaVersion, OutError))
	{
		return false;
	}
	if (OutManifest.SchemaVersion != RequiredSchemaVersion)
	{
		OutError = FString::Printf(
			TEXT("Unsupported schema version '%s'. Required '%s'."),
			*OutManifest.SchemaVersion,
			RequiredSchemaVersion);
		return false;
	}

	SFMapJsonPrivate::RequireString(Root, TEXT("projection"), OutManifest.Projection, OutError);
	SFMapJsonPrivate::RequireString(Root, TEXT("graph_file"), OutManifest.GraphFile, OutError);
	SFMapJsonPrivate::RequireString(Root, TEXT("landmarks_file"), OutManifest.LandmarksFile, OutError);
	SFMapJsonPrivate::RequireString(Root, TEXT("races_file"), OutManifest.RacesFile, OutError);

	double TileSize = 0.0;
	double TileCount = 0.0;
	double RoadCount = 0.0;
	double BuildingCount = 0.0;
	double LandmarkCount = 0.0;
	double RaceCount = 0.0;
	SFMapJsonPrivate::GetNumber(Root, TEXT("tile_size_meters"), TileSize);
	SFMapJsonPrivate::GetNumber(Root, TEXT("tile_count"), TileCount);
	SFMapJsonPrivate::GetNumber(Root, TEXT("road_count"), RoadCount);
	SFMapJsonPrivate::GetNumber(Root, TEXT("building_count"), BuildingCount);
	SFMapJsonPrivate::GetNumber(Root, TEXT("landmark_count"), LandmarkCount);
	SFMapJsonPrivate::GetNumber(Root, TEXT("race_count"), RaceCount);
	OutManifest.TileSizeMeters = static_cast<int32>(TileSize);
	OutManifest.TileCount = static_cast<int32>(TileCount);
	OutManifest.RoadCount = static_cast<int32>(RoadCount);
	OutManifest.BuildingCount = static_cast<int32>(BuildingCount);
	OutManifest.LandmarkCount = static_cast<int32>(LandmarkCount);
	OutManifest.RaceCount = static_cast<int32>(RaceCount);

	if (const TSharedPtr<FJsonObject>* OriginObject = nullptr; Root->TryGetObjectField(TEXT("world_origin"), OriginObject) && OriginObject && OriginObject->IsValid())
	{
		SFMapJsonPrivate::GetNumber(*OriginObject, TEXT("latitude"), OutManifest.WorldOrigin.Latitude);
		SFMapJsonPrivate::GetNumber(*OriginObject, TEXT("longitude"), OutManifest.WorldOrigin.Longitude);
	}

	if (const TSharedPtr<FJsonObject>* BBoxObject = nullptr; Root->TryGetObjectField(TEXT("bbox"), BBoxObject) && BBoxObject && BBoxObject->IsValid())
	{
		SFMapJsonPrivate::GetNumber(*BBoxObject, TEXT("west"), OutManifest.BBox.West);
		SFMapJsonPrivate::GetNumber(*BBoxObject, TEXT("south"), OutManifest.BBox.South);
		SFMapJsonPrivate::GetNumber(*BBoxObject, TEXT("east"), OutManifest.BBox.East);
		SFMapJsonPrivate::GetNumber(*BBoxObject, TEXT("north"), OutManifest.BBox.North);
	}

	const TArray<TSharedPtr<FJsonValue>>* TileFiles = nullptr;
	if (Root->TryGetArrayField(TEXT("tile_files"), TileFiles) && TileFiles)
	{
		for (const TSharedPtr<FJsonValue>& Value : *TileFiles)
		{
			OutManifest.TileFiles.Add(Value->AsString());
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* Attribution = nullptr;
	if (Root->TryGetArrayField(TEXT("attribution"), Attribution) && Attribution)
	{
		for (const TSharedPtr<FJsonValue>& Value : *Attribution)
		{
			OutManifest.Attribution.Add(Value->AsString());
		}
	}

	if (const TSharedPtr<FJsonObject>* Versions = nullptr; Root->TryGetObjectField(TEXT("source_dataset_versions"), Versions) && Versions && Versions->IsValid())
	{
		for (const auto& Pair : (*Versions)->Values)
		{
			OutManifest.SourceDatasetVersions.Add(FString(Pair.Key), Pair.Value->AsString());
		}
	}

	return true;
}

bool FSFMapJsonLoader::LoadGraph(
	const FString& GraphPath,
	TArray<FSFRoadNodeData>& OutNodes,
	TArray<FSFRoadEdgeData>& OutEdges,
	FString& OutError)
{
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(GraphPath, Root, OutError))
	{
		return false;
	}

	FString Schema;
	if (Root->TryGetStringField(TEXT("schema_version"), Schema) && Schema != RequiredSchemaVersion)
	{
		OutError = FString::Printf(TEXT("Graph schema version mismatch: %s"), *Schema);
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* NodesArray = nullptr;
	if (!Root->TryGetArrayField(TEXT("nodes"), NodesArray) || !NodesArray)
	{
		OutError = TEXT("Graph is missing nodes array");
		return false;
	}

	TSet<FString> NodeIds;
	for (const TSharedPtr<FJsonValue>& Value : *NodesArray)
	{
		const TSharedPtr<FJsonObject> Object = Value->AsObject();
		FSFRoadNodeData Node;
		if (!SFMapJsonPrivate::RequireString(Object, TEXT("id"), Node.Id, OutError))
		{
			return false;
		}
		if (NodeIds.Contains(Node.Id))
		{
			OutError = FString::Printf(TEXT("Duplicate node id '%s'"), *Node.Id);
			return false;
		}
		SFMapJsonPrivate::GetNumber(Object, TEXT("x_m"), Node.XMeters);
		SFMapJsonPrivate::GetNumber(Object, TEXT("y_m"), Node.YMeters);
		SFMapJsonPrivate::GetNumber(Object, TEXT("elevation_m"), Node.ElevationMeters);
		NodeIds.Add(Node.Id);
		OutNodes.Add(Node);
	}

	const TArray<TSharedPtr<FJsonValue>>* EdgesArray = nullptr;
	if (!Root->TryGetArrayField(TEXT("edges"), EdgesArray) || !EdgesArray)
	{
		OutError = TEXT("Graph is missing edges array");
		return false;
	}

	TSet<FString> EdgeIds;
	for (const TSharedPtr<FJsonValue>& Value : *EdgesArray)
	{
		const TSharedPtr<FJsonObject> Object = Value->AsObject();
		FSFRoadEdgeData Edge;
		if (!SFMapJsonPrivate::RequireString(Object, TEXT("id"), Edge.Id, OutError))
		{
			return false;
		}
		if (EdgeIds.Contains(Edge.Id))
		{
			OutError = FString::Printf(TEXT("Duplicate edge id '%s'"), *Edge.Id);
			return false;
		}
		if (!SFMapJsonPrivate::RequireString(Object, TEXT("from"), Edge.FromNodeId, OutError)
			|| !SFMapJsonPrivate::RequireString(Object, TEXT("to"), Edge.ToNodeId, OutError))
		{
			return false;
		}
		if (!NodeIds.Contains(Edge.FromNodeId) || !NodeIds.Contains(Edge.ToNodeId))
		{
			OutError = FString::Printf(TEXT("Edge '%s' references missing nodes"), *Edge.Id);
			return false;
		}

		Object->TryGetStringField(TEXT("road_class"), Edge.RoadClass);
		double Length = 0.0;
		double Travel = 0.0;
		double LaneWidth = 0.0;
		double TotalWidth = 0.0;
		double LaneCount = 0.0;
		double Speed = 0.0;
		double Layer = 0.0;
		SFMapJsonPrivate::GetNumber(Object, TEXT("length_m"), Length);
		SFMapJsonPrivate::GetNumber(Object, TEXT("travel_time_s"), Travel);
		SFMapJsonPrivate::GetNumber(Object, TEXT("lane_width_m"), LaneWidth);
		SFMapJsonPrivate::GetNumber(Object, TEXT("total_width_m"), TotalWidth);
		SFMapJsonPrivate::GetNumber(Object, TEXT("lane_count"), LaneCount);
		SFMapJsonPrivate::GetNumber(Object, TEXT("speed_limit_kph"), Speed);
		SFMapJsonPrivate::GetNumber(Object, TEXT("layer"), Layer);
		Edge.LengthMeters = static_cast<float>(Length);
		Edge.TravelTimeSeconds = static_cast<float>(Travel);
		Edge.LaneWidthMeters = static_cast<float>(LaneWidth);
		Edge.TotalWidthMeters = static_cast<float>(TotalWidth);
		Edge.LaneCount = FMath::Max(1, static_cast<int32>(LaneCount));
		Edge.SpeedLimitKph = FMath::Max(1, static_cast<int32>(Speed));
		Edge.Layer = static_cast<int32>(Layer);
		Object->TryGetBoolField(TEXT("one_way"), Edge.bOneWay);
		Object->TryGetBoolField(TEXT("bridge"), Edge.bBridge);
		Object->TryGetBoolField(TEXT("tunnel"), Edge.bTunnel);
		if (!Object->TryGetBoolField(TEXT("driveable"), Edge.bDriveable))
		{
			Edge.bDriveable = true;
		}

		const TArray<TSharedPtr<FJsonValue>>* Points = nullptr;
		if (Object->TryGetArrayField(TEXT("points"), Points) && Points)
		{
			for (const TSharedPtr<FJsonValue>& PointValue : *Points)
			{
				const TArray<TSharedPtr<FJsonValue>> Coords = PointValue->AsArray();
				if (Coords.Num() < 2)
				{
					OutError = FString::Printf(TEXT("Edge '%s' has invalid point geometry"), *Edge.Id);
					return false;
				}
				const double X = Coords[0]->AsNumber();
				const double Y = Coords[1]->AsNumber();
				const double Z = Coords.Num() > 2 ? Coords[2]->AsNumber() : 0.0;
				Edge.PointsLocalMeters.Add(FVector(X, Y, Z));
			}
		}

		if (Edge.PointsLocalMeters.Num() < 2 || Edge.LengthMeters <= 0.0f)
		{
			OutError = FString::Printf(TEXT("Edge '%s' has invalid geometry or length"), *Edge.Id);
			return false;
		}

		EdgeIds.Add(Edge.Id);
		OutEdges.Add(Edge);
	}

	return true;
}

bool FSFMapJsonLoader::LoadLandmarks(const FString& Path, TArray<FSFLandmarkData>& OutLandmarks, FString& OutError)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	if (!ReadJsonArrayFile(Path, Array, OutError))
	{
		return false;
	}

	TSet<FString> Ids;
	for (const TSharedPtr<FJsonValue>& Value : Array)
	{
		const TSharedPtr<FJsonObject> Object = Value->AsObject();
		FSFLandmarkData Landmark;
		if (!SFMapJsonPrivate::RequireString(Object, TEXT("id"), Landmark.Id, OutError)
			|| !SFMapJsonPrivate::RequireString(Object, TEXT("name"), Landmark.Name, OutError))
		{
			return false;
		}
		if (Ids.Contains(Landmark.Id))
		{
			OutError = FString::Printf(TEXT("Duplicate landmark id '%s'"), *Landmark.Id);
			return false;
		}
		SFMapJsonPrivate::GetNumber(Object, TEXT("latitude"), Landmark.Latitude);
		SFMapJsonPrivate::GetNumber(Object, TEXT("longitude"), Landmark.Longitude);
		Object->TryGetBoolField(TEXT("enabled"), Landmark.bEnabled);

		if (const TSharedPtr<FJsonObject>* SpawnObject = nullptr; Object->TryGetObjectField(TEXT("spawn"), SpawnObject) && SpawnObject && SpawnObject->IsValid())
		{
			Landmark.bHasSpawn = true;
			SFMapJsonPrivate::RequireString(*SpawnObject, TEXT("edge_id"), Landmark.Spawn.EdgeId, OutError);
			SFMapJsonPrivate::RequireString(*SpawnObject, TEXT("node_id"), Landmark.Spawn.NodeId, OutError);
			SFMapJsonPrivate::GetNumber(*SpawnObject, TEXT("x_m"), Landmark.Spawn.XMeters);
			SFMapJsonPrivate::GetNumber(*SpawnObject, TEXT("y_m"), Landmark.Spawn.YMeters);
			double Heading = 0.0;
			SFMapJsonPrivate::GetNumber(*SpawnObject, TEXT("heading_degrees"), Heading);
			Landmark.Spawn.HeadingDegrees = static_cast<float>(Heading);
		}

		Ids.Add(Landmark.Id);
		OutLandmarks.Add(Landmark);
	}
	return true;
}

bool FSFMapJsonLoader::LoadRaces(const FString& Path, TArray<FSFRaceDefinitionData>& OutRaces, FString& OutError)
{
	TArray<TSharedPtr<FJsonValue>> Array;
	if (!ReadJsonArrayFile(Path, Array, OutError))
	{
		return false;
	}

	for (const TSharedPtr<FJsonValue>& Value : Array)
	{
		const TSharedPtr<FJsonObject> Object = Value->AsObject();
		FSFRaceDefinitionData Race;
		if (!SFMapJsonPrivate::RequireString(Object, TEXT("race_id"), Race.RaceId, OutError)
			|| !SFMapJsonPrivate::RequireString(Object, TEXT("start_landmark_id"), Race.StartLandmarkId, OutError)
			|| !SFMapJsonPrivate::RequireString(Object, TEXT("destination_landmark_id"), Race.DestinationLandmarkId, OutError))
		{
			return false;
		}

		const TArray<TSharedPtr<FJsonValue>>* Routes = nullptr;
		if (!Object->TryGetArrayField(TEXT("routes"), Routes) || !Routes || Routes->Num() < 3)
		{
			OutError = FString::Printf(TEXT("Race '%s' must include at least three routes"), *Race.RaceId);
			return false;
		}

		for (const TSharedPtr<FJsonValue>& RouteValue : *Routes)
		{
			const TSharedPtr<FJsonObject> RouteObject = RouteValue->AsObject();
			FSFSuggestedRouteData Route;
			SFMapJsonPrivate::RequireString(RouteObject, TEXT("profile"), Route.Profile, OutError);
			double Distance = 0.0;
			double Time = 0.0;
			double Overlap = 0.0;
			SFMapJsonPrivate::GetNumber(RouteObject, TEXT("distance_m"), Distance);
			SFMapJsonPrivate::GetNumber(RouteObject, TEXT("estimated_time_s"), Time);
			SFMapJsonPrivate::GetNumber(RouteObject, TEXT("overlap_with_fastest"), Overlap);
			Route.DistanceMeters = static_cast<float>(Distance);
			Route.EstimatedTimeSeconds = static_cast<float>(Time);
			Route.OverlapWithFastest = static_cast<float>(Overlap);

			const TArray<TSharedPtr<FJsonValue>>* EdgeIds = nullptr;
			if (RouteObject->TryGetArrayField(TEXT("edge_ids"), EdgeIds) && EdgeIds)
			{
				for (const TSharedPtr<FJsonValue>& EdgeId : *EdgeIds)
				{
					Route.EdgeIds.Add(EdgeId->AsString());
				}
			}
			const TArray<TSharedPtr<FJsonValue>>* NodeIds = nullptr;
			if (RouteObject->TryGetArrayField(TEXT("node_ids"), NodeIds) && NodeIds)
			{
				for (const TSharedPtr<FJsonValue>& NodeId : *NodeIds)
				{
					Route.NodeIds.Add(NodeId->AsString());
				}
			}
			Race.Routes.Add(Route);
		}
		OutRaces.Add(Race);
	}
	return true;
}

bool FSFMapJsonLoader::LoadTile(const FString& Path, FSFMapTileData& OutTile, FString& OutError)
{
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(Path, Root, OutError))
	{
		return false;
	}

	if (!SFMapJsonPrivate::RequireString(Root, TEXT("tile_id"), OutTile.TileId, OutError))
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* Roads = nullptr;
	if (Root->TryGetArrayField(TEXT("roads"), Roads) && Roads)
	{
		for (const TSharedPtr<FJsonValue>& Road : *Roads)
		{
			OutTile.RoadIds.Add(Road->AsString());
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* Buildings = nullptr;
	if (Root->TryGetArrayField(TEXT("buildings"), Buildings) && Buildings)
	{
		for (const TSharedPtr<FJsonValue>& BuildingValue : *Buildings)
		{
			const TSharedPtr<FJsonObject> BuildingObject = BuildingValue->AsObject();
			FSFBuildingData Building;
			SFMapJsonPrivate::RequireString(BuildingObject, TEXT("id"), Building.Id, OutError);
			BuildingObject->TryGetStringField(TEXT("category"), Building.Category);
			BuildingObject->TryGetStringField(TEXT("tile_id"), Building.TileId);
			double Height = 0.0;
			SFMapJsonPrivate::GetNumber(BuildingObject, TEXT("height_m"), Height);
			Building.HeightMeters = static_cast<float>(Height);

			const TArray<TSharedPtr<FJsonValue>>* Outer = nullptr;
			if (BuildingObject->TryGetArrayField(TEXT("outer_ring"), Outer) && Outer)
			{
				for (const TSharedPtr<FJsonValue>& PointValue : *Outer)
				{
					const TArray<TSharedPtr<FJsonValue>> Coords = PointValue->AsArray();
					if (Coords.Num() >= 2)
					{
						Building.OuterRingLocalMeters.Add(FVector2D(Coords[0]->AsNumber(), Coords[1]->AsNumber()));
					}
				}
			}
			OutTile.Buildings.Add(Building);
		}
	}
	return true;
}

bool FSFMapJsonLoader::ValidateLoadedData(const FSFMapLoadResult& Result, FString& OutError)
{
	if (Result.Manifest.SchemaVersion != RequiredSchemaVersion)
	{
		OutError = TEXT("Manifest schema version is unsupported");
		return false;
	}

	TMap<FString, const FSFRoadEdgeData*> EdgeLookup;
	for (const FSFRoadEdgeData& Edge : Result.Edges)
	{
		EdgeLookup.Add(Edge.Id, &Edge);
	}

	for (const FSFLandmarkData& Landmark : Result.Landmarks)
	{
		if (Landmark.bEnabled && !Landmark.bHasSpawn)
		{
			OutError = FString::Printf(TEXT("Enabled landmark '%s' is missing a spawn"), *Landmark.Id);
			return false;
		}
		if (Landmark.bHasSpawn && !EdgeLookup.Contains(Landmark.Spawn.EdgeId))
		{
			OutError = FString::Printf(TEXT("Landmark '%s' spawn references missing edge"), *Landmark.Id);
			return false;
		}
	}

	for (const FSFRaceDefinitionData& Race : Result.Races)
	{
		for (const FSFSuggestedRouteData& Route : Race.Routes)
		{
			for (const FString& EdgeId : Route.EdgeIds)
			{
				const FSFRoadEdgeData* const* Edge = EdgeLookup.Find(EdgeId);
				if (!Edge || !*Edge)
				{
					OutError = FString::Printf(TEXT("Race '%s' route references missing edge '%s'"), *Race.RaceId, *EdgeId);
					return false;
				}
				if (!(*Edge)->bDriveable)
				{
					OutError = FString::Printf(TEXT("Race '%s' route uses non-driveable edge '%s'"), *Race.RaceId, *EdgeId);
					return false;
				}
			}
		}
	}

	TSet<FString> TileIds;
	for (const FSFMapTileData& Tile : Result.Tiles)
	{
		if (TileIds.Contains(Tile.TileId))
		{
			OutError = FString::Printf(TEXT("Duplicate tile id '%s'"), *Tile.TileId);
			return false;
		}
		TileIds.Add(Tile.TileId);
		for (const FString& RoadId : Tile.RoadIds)
		{
			if (!EdgeLookup.Contains(RoadId))
			{
				OutError = FString::Printf(TEXT("Tile '%s' references missing road '%s'"), *Tile.TileId, *RoadId);
				return false;
			}
		}
	}

	if (Result.Manifest.TileFiles.Num() != Result.Tiles.Num())
	{
		OutError = TEXT("Manifest tile file count does not match loaded tiles");
		return false;
	}

	return true;
}

bool FSFMapJsonLoader::LoadDirectory(const FString& ExportDirectory, FSFMapLoadResult& OutResult)
{
	OutResult = FSFMapLoadResult();
	const FString ManifestPath = FPaths::Combine(ExportDirectory, TEXT("manifest.json"));
	if (!LoadManifest(ManifestPath, OutResult.Manifest, OutResult.ErrorMessage))
	{
		UE_LOG(LogSFGeo, Error, TEXT("%s"), *OutResult.ErrorMessage);
		return false;
	}

	const FString GraphPath = FPaths::Combine(ExportDirectory, OutResult.Manifest.GraphFile);
	if (!LoadGraph(GraphPath, OutResult.Nodes, OutResult.Edges, OutResult.ErrorMessage))
	{
		UE_LOG(LogSFGeo, Error, TEXT("%s"), *OutResult.ErrorMessage);
		return false;
	}

	const FString LandmarksPath = FPaths::Combine(ExportDirectory, OutResult.Manifest.LandmarksFile);
	if (!LoadLandmarks(LandmarksPath, OutResult.Landmarks, OutResult.ErrorMessage))
	{
		UE_LOG(LogSFGeo, Error, TEXT("%s"), *OutResult.ErrorMessage);
		return false;
	}

	const FString RacesPath = FPaths::Combine(ExportDirectory, OutResult.Manifest.RacesFile);
	if (!LoadRaces(RacesPath, OutResult.Races, OutResult.ErrorMessage))
	{
		UE_LOG(LogSFGeo, Error, TEXT("%s"), *OutResult.ErrorMessage);
		return false;
	}

	for (const FString& RelativeTile : OutResult.Manifest.TileFiles)
	{
		FSFMapTileData Tile;
		const FString TilePath = FPaths::Combine(ExportDirectory, RelativeTile);
		if (!LoadTile(TilePath, Tile, OutResult.ErrorMessage))
		{
			UE_LOG(LogSFGeo, Error, TEXT("%s"), *OutResult.ErrorMessage);
			return false;
		}
		OutResult.Tiles.Add(Tile);
	}

	if (!ValidateLoadedData(OutResult, OutResult.ErrorMessage))
	{
		UE_LOG(LogSFGeo, Error, TEXT("Map validation failed: %s"), *OutResult.ErrorMessage);
		return false;
	}

	OutResult.bSuccess = true;
	UE_LOG(
		LogSFGeo,
		Log,
		TEXT("Loaded SF map export: %d roads, %d landmarks, %d races, %d tiles"),
		OutResult.Edges.Num(),
		OutResult.Landmarks.Num(),
		OutResult.Races.Num(),
		OutResult.Tiles.Num());
	return true;
}
