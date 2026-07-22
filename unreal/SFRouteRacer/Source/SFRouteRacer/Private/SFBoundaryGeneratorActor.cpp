#include "SFBoundaryGeneratorActor.h"

#include "Engine/GameInstance.h"
#include "SFBoundaryBarrierActor.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "SFRouteRacer.h"

ASFBoundaryGeneratorActor::ASFBoundaryGeneratorActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASFBoundaryGeneratorActor::BeginPlay()
{
	Super::BeginPlay();
	if (bGenerateOnBeginPlay)
	{
		GenerateFromLoadedMap();
	}
}

void ASFBoundaryGeneratorActor::GenerateFromLoadedMap()
{
	for (ASFBoundaryBarrierActor* Barrier : SpawnedBarriers)
	{
		if (Barrier)
		{
			Barrier->Destroy();
		}
	}
	SpawnedBarriers.Reset();

	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Warning, TEXT("Boundary generator skipped: map data unavailable"));
		return;
	}

	// Approximate local bbox from loaded node extents.
	double MinX = TNumericLimits<double>::Max();
	double MaxX = TNumericLimits<double>::Lowest();
	double MinY = TNumericLimits<double>::Max();
	double MaxY = TNumericLimits<double>::Lowest();
	for (const FSFRoadNodeData& Node : MapData->GetNodes())
	{
		MinX = FMath::Min(MinX, Node.XMeters);
		MaxX = FMath::Max(MaxX, Node.XMeters);
		MinY = FMath::Min(MinY, Node.YMeters);
		MaxY = FMath::Max(MaxY, Node.YMeters);
	}
	MinX += InsetMeters;
	MaxX -= InsetMeters;
	MinY += InsetMeters;
	MaxY -= InsetMeters;

	struct FWall
	{
		FVector Location;
		FRotator Rotation;
		float LengthCm;
	};

	const FVector SW = USFGeoCoordinateLibrary::Point2DLocalToUnreal(MinX, MinY, 0.0);
	const FVector SE = USFGeoCoordinateLibrary::Point2DLocalToUnreal(MaxX, MinY, 0.0);
	const FVector NW = USFGeoCoordinateLibrary::Point2DLocalToUnreal(MinX, MaxY, 0.0);
	const FVector NE = USFGeoCoordinateLibrary::Point2DLocalToUnreal(MaxX, MaxY, 0.0);

	TArray<FWall> Walls;
	Walls.Add({(SW + SE) * 0.5f, (SE - SW).Rotation(), FVector::Dist(SW, SE)});
	Walls.Add({(NW + NE) * 0.5f, (NE - NW).Rotation(), FVector::Dist(NW, NE)});
	Walls.Add({(SW + NW) * 0.5f, (NW - SW).Rotation(), FVector::Dist(SW, NW)});
	Walls.Add({(SE + NE) * 0.5f, (NE - SE).Rotation(), FVector::Dist(SE, NE)});

	FActorSpawnParameters Params;
	Params.Owner = this;
	for (const FWall& Wall : Walls)
	{
		ASFBoundaryBarrierActor* Barrier = GetWorld()->SpawnActor<ASFBoundaryBarrierActor>(
			ASFBoundaryBarrierActor::StaticClass(), Wall.Location, Wall.Rotation, Params);
		if (Barrier)
		{
			Barrier->Configure(Wall.Location + FVector(0, 0, BarrierHeightCm * 0.5f), Wall.Rotation, Wall.LengthCm, BarrierHeightCm);
			SpawnedBarriers.Add(Barrier);
		}
	}

	UE_LOG(LogSFRace, Log, TEXT("Spawned %d MVP boundary barriers"), SpawnedBarriers.Num());
}
