#include "SFMapDataSubsystem.h"

#include "Misc/Paths.h"
#include "SFGeoRuntime.h"
#include "SFMapJsonLoader.h"

void USFMapDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

FString USFMapDataSubsystem::ResolveDefaultExportDirectory()
{
	const FString ProjectDir = FPaths::ProjectDir();
	const FString CandidateA = FPaths::ConvertRelativePathToFull(FPaths::Combine(ProjectDir, TEXT("../../data/exports/sf_mvp")));
	const FString CandidateB = FPaths::ConvertRelativePathToFull(FPaths::Combine(ProjectDir, TEXT("../../../data/exports/sf_mvp")));
	const FString CandidateC = FPaths::ConvertRelativePathToFull(FPaths::Combine(ProjectDir, TEXT("Content/Maps/sf_mvp")));

	if (FPaths::FileExists(FPaths::Combine(CandidateA, TEXT("manifest.json"))))
	{
		return CandidateA;
	}
	if (FPaths::FileExists(FPaths::Combine(CandidateB, TEXT("manifest.json"))))
	{
		return CandidateB;
	}
	return CandidateC;
}

bool USFMapDataSubsystem::LoadExportDirectory(const FString& ExportDirectory)
{
	bMapLoaded = false;
	LastError.Reset();
	Nodes.Reset();
	Edges.Reset();
	Landmarks.Reset();
	Races.Reset();
	Tiles.Reset();
	EdgeIndexById.Reset();
	LandmarkIndexById.Reset();
	RaceIndexById.Reset();

	FSFMapLoadResult Result;
	if (!FSFMapJsonLoader::LoadDirectory(ExportDirectory, Result))
	{
		LastError = Result.ErrorMessage;
		return false;
	}

	Manifest = Result.Manifest;
	Nodes = MoveTemp(Result.Nodes);
	Edges = MoveTemp(Result.Edges);
	Landmarks = MoveTemp(Result.Landmarks);
	Races = MoveTemp(Result.Races);
	Tiles = MoveTemp(Result.Tiles);

	for (int32 Index = 0; Index < Edges.Num(); ++Index)
	{
		EdgeIndexById.Add(Edges[Index].Id, Index);
	}
	for (int32 Index = 0; Index < Landmarks.Num(); ++Index)
	{
		LandmarkIndexById.Add(Landmarks[Index].Id, Index);
	}
	for (int32 Index = 0; Index < Races.Num(); ++Index)
	{
		RaceIndexById.Add(Races[Index].RaceId, Index);
	}

	bMapLoaded = true;
	return true;
}

bool USFMapDataSubsystem::LoadDefaultExport()
{
	return LoadExportDirectory(ResolveDefaultExportDirectory());
}

bool USFMapDataSubsystem::FindLandmark(const FString& LandmarkId, FSFLandmarkData& OutLandmark) const
{
	if (const int32* Index = LandmarkIndexById.Find(LandmarkId))
	{
		OutLandmark = Landmarks[*Index];
		return true;
	}
	return false;
}

bool USFMapDataSubsystem::FindRace(const FString& RaceId, FSFRaceDefinitionData& OutRace) const
{
	if (const int32* Index = RaceIndexById.Find(RaceId))
	{
		OutRace = Races[*Index];
		return true;
	}
	return false;
}

bool USFMapDataSubsystem::FindEdge(const FString& EdgeId, FSFRoadEdgeData& OutEdge) const
{
	if (const int32* Index = EdgeIndexById.Find(EdgeId))
	{
		OutEdge = Edges[*Index];
		return true;
	}
	return false;
}
