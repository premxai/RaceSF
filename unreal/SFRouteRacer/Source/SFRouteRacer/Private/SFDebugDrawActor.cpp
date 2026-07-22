#include "SFDebugDrawActor.h"

#include "DrawDebugHelpers.h"
#include "Engine/GameInstance.h"
#include "SFDestinationMarker.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

ASFDebugDrawActor::ASFDebugDrawActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASFDebugDrawActor::BeginPlay()
{
	Super::BeginPlay();
#if UE_BUILD_SHIPPING
	bDebugEnabled = false;
	SetActorTickEnabled(false);
#endif
}

void ASFDebugDrawActor::SetDebugEnabled(bool bEnabled)
{
#if UE_BUILD_SHIPPING
	bDebugEnabled = false;
#else
	bDebugEnabled = bEnabled;
#endif
}

void ASFDebugDrawActor::SetShowGraph(bool bEnabled) { bShowGraph = bEnabled; }
void ASFDebugDrawActor::SetShowLandmarks(bool bEnabled) { bShowLandmarks = bEnabled; }
void ASFDebugDrawActor::SetShowTiles(bool bEnabled) { bShowTiles = bEnabled; }
void ASFDebugDrawActor::SetShowDestinationRadius(bool bEnabled) { bShowDestinationRadius = bEnabled; }

void ASFDebugDrawActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
#if UE_BUILD_SHIPPING
	return;
#else
	if (!bDebugEnabled)
	{
		return;
	}
	if (bShowGraph)
	{
		DrawGraph();
	}
	if (bShowLandmarks)
	{
		DrawLandmarks();
	}
	if (bShowTiles)
	{
		DrawTiles();
	}
	if (bShowDestinationRadius)
	{
		DrawDestinationRadius();
	}
#endif
}

void ASFDebugDrawActor::DrawGraph() const
{
	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || !MapData->IsMapLoaded())
	{
		return;
	}

	int32 Drawn = 0;
	for (const FSFRoadEdgeData& Edge : MapData->GetEdges())
	{
		if (!Edge.bDriveable || Edge.PointsLocalMeters.Num() < 2)
		{
			continue;
		}
		if (MaxEdgesToDraw > 0 && Drawn >= MaxEdgesToDraw)
		{
			break;
		}
		const FColor Color = Edge.bOneWay ? FColor::Cyan : FColor::White;
		for (int32 Index = 0; Index + 1 < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector A = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index]) + FVector(0, 0, 30);
			const FVector B = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index + 1]) + FVector(0, 0, 30);
			DrawDebugLine(GetWorld(), A, B, Color, false, -1.0f, 0, 4.0f);
		}
		++Drawn;
	}
}

void ASFDebugDrawActor::DrawLandmarks() const
{
	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || !MapData->IsMapLoaded())
	{
		return;
	}

	for (const FSFLandmarkData& Landmark : MapData->GetLandmarks())
	{
		if (!Landmark.bHasSpawn)
		{
			continue;
		}
		const FVector Spawn = USFGeoCoordinateLibrary::Point2DLocalToUnreal(
			Landmark.Spawn.XMeters, Landmark.Spawn.YMeters, 0.0) + FVector(0, 0, 120);
		DrawDebugSphere(GetWorld(), Spawn, 120.0f, 12, FColor::Orange, false, -1.0f, 0, 2.0f);
		DrawDebugString(GetWorld(), Spawn + FVector(0, 0, 80), Landmark.Name, nullptr, FColor::White, 0.0f, false);
	}
}

void ASFDebugDrawActor::DrawTiles() const
{
	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || !MapData->IsMapLoaded())
	{
		return;
	}

	const int32 TileSize = MapData->GetManifest().TileSizeMeters;
	for (const FSFMapTileData& Tile : MapData->GetTiles())
	{
		// tile_x_y
		TArray<FString> Parts;
		Tile.TileId.ParseIntoArray(Parts, TEXT("_"), true);
		if (Parts.Num() < 3)
		{
			continue;
		}
		const int32 TX = FCString::Atoi(*Parts[1]);
		const int32 TY = FCString::Atoi(*Parts[2]);
		const FVector Min = USFGeoCoordinateLibrary::Point2DLocalToUnreal(TX * TileSize, TY * TileSize, 0.0);
		const FVector Max = USFGeoCoordinateLibrary::Point2DLocalToUnreal((TX + 1) * TileSize, (TY + 1) * TileSize, 0.0);
		const FVector Center = (Min + Max) * 0.5f + FVector(0, 0, 20);
		const FVector Extent(FMath::Abs(Max.X - Min.X) * 0.5f, FMath::Abs(Max.Y - Min.Y) * 0.5f, 10.0f);
		DrawDebugBox(GetWorld(), Center, Extent, FColor::Purple, false, -1.0f, 0, 2.0f);
	}
}

void ASFDebugDrawActor::DrawDestinationRadius() const
{
	AActor* Marker = UGameplayStatics::GetActorOfClass(GetWorld(), ASFDestinationMarker::StaticClass());
	if (!Marker)
	{
		return;
	}
	DrawDebugSphere(
		GetWorld(),
		Marker->GetActorLocation(),
		DestinationRadiusMeters * 100.0f,
		24,
		FColor(255, 176, 0),
		false,
		-1.0f,
		0,
		3.0f);
}
