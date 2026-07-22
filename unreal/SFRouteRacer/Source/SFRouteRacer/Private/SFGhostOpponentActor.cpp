#include "SFGhostOpponentActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "SFRouteRacer.h"
#include "UObject/ConstructorHelpers.h"

ASFGhostOpponentActor::ASFGhostOpponentActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh"));
	SetRootComponent(GhostMesh);
	GhostMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMesh->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		GhostMesh->SetStaticMesh(CubeMesh.Object);
		GhostMesh->SetRelativeScale3D(FVector(2.2f, 1.1f, 0.7f));
		GhostMesh->SetVisibility(true);
	}
}

void ASFGhostOpponentActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
}

void ASFGhostOpponentActor::StopGhost()
{
	bRacing = false;
	SetActorHiddenInGame(true);
}

float ASFGhostOpponentActor::GetProgressAlpha() const
{
	return TotalPathLengthMeters > 0.0f
		? FMath::Clamp(DistanceAlongPathMeters / TotalPathLengthMeters, 0.0f, 1.0f)
		: 0.0f;
}

void ASFGhostOpponentActor::RebuildPathSamples(const TArray<FString>& EdgeIds)
{
	PathPointsUnreal.Reset();
	CumulativeDistancesMeters.Reset();
	TotalPathLengthMeters = 0.0f;

	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData)
	{
		return;
	}

	for (const FString& EdgeId : EdgeIds)
	{
		FSFRoadEdgeData Edge;
		if (!MapData->FindEdge(EdgeId, Edge))
		{
			continue;
		}
		for (int32 Index = 0; Index < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector Point = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index])
				+ FVector(0, 0, 60.0f);
			if (PathPointsUnreal.Num() > 0)
			{
				const float SegmentMeters = FVector::Dist(PathPointsUnreal.Last(), Point) * 0.01f;
				// Skip near-duplicates at edge joints.
				if (SegmentMeters < 0.05f)
				{
					continue;
				}
				TotalPathLengthMeters += SegmentMeters;
			}
			PathPointsUnreal.Add(Point);
			CumulativeDistancesMeters.Add(TotalPathLengthMeters);
		}
	}
}

FTransform ASFGhostOpponentActor::SampleTransformAtDistance(float DistanceMeters) const
{
	if (PathPointsUnreal.Num() == 0)
	{
		return GetActorTransform();
	}
	if (PathPointsUnreal.Num() == 1 || DistanceMeters <= 0.0f)
	{
		FVector Forward = FVector::ForwardVector;
		if (PathPointsUnreal.Num() > 1)
		{
			Forward = (PathPointsUnreal[1] - PathPointsUnreal[0]).GetSafeNormal();
		}
		return FTransform(Forward.Rotation(), PathPointsUnreal[0]);
	}

	const float Clamped = FMath::Clamp(DistanceMeters, 0.0f, TotalPathLengthMeters);
	for (int32 Index = 1; Index < CumulativeDistancesMeters.Num(); ++Index)
	{
		if (Clamped <= CumulativeDistancesMeters[Index])
		{
			const float SegStart = CumulativeDistancesMeters[Index - 1];
			const float SegEnd = CumulativeDistancesMeters[Index];
			const float Alpha = SegEnd > SegStart ? (Clamped - SegStart) / (SegEnd - SegStart) : 0.0f;
			const FVector Location = FMath::Lerp(PathPointsUnreal[Index - 1], PathPointsUnreal[Index], Alpha);
			const FVector Forward = (PathPointsUnreal[Index] - PathPointsUnreal[Index - 1]).GetSafeNormal();
			return FTransform(Forward.Rotation(), Location);
		}
	}

	const FVector Forward = (PathPointsUnreal.Last() - PathPointsUnreal[PathPointsUnreal.Num() - 2]).GetSafeNormal();
	return FTransform(Forward.Rotation(), PathPointsUnreal.Last());
}

bool ASFGhostOpponentActor::StartGhostForRace(const FString& RaceId, const FString& Profile)
{
	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Error, TEXT("Ghost opponent cannot start: map data missing"));
		return false;
	}

	FSFRaceDefinitionData Race;
	if (!MapData->FindRace(RaceId, Race))
	{
		UE_LOG(LogSFRace, Error, TEXT("Ghost opponent unknown race %s"), *RaceId);
		return false;
	}

	const FSFSuggestedRouteData* Chosen = nullptr;
	for (const FSFSuggestedRouteData& Route : Race.Routes)
	{
		if (Route.Profile.Equals(Profile, ESearchCase::IgnoreCase))
		{
			Chosen = &Route;
			break;
		}
	}
	// Prefer a non-fastest route when available.
	if (!Chosen)
	{
		for (const FSFSuggestedRouteData& Route : Race.Routes)
		{
			if (!Route.Profile.Equals(TEXT("fastest"), ESearchCase::IgnoreCase))
			{
				Chosen = &Route;
				break;
			}
		}
	}
	if (!Chosen && Race.Routes.Num() > 0)
	{
		Chosen = &Race.Routes[0];
	}
	if (!Chosen)
	{
		return false;
	}

	RebuildPathSamples(Chosen->EdgeIds);
	if (PathPointsUnreal.Num() < 2)
	{
		UE_LOG(LogSFRace, Error, TEXT("Ghost opponent path is empty"));
		return false;
	}

	DistanceAlongPathMeters = 0.0f;
	StuckTimer = 0.0f;
	bRacing = true;
	SetActorHiddenInGame(false);
	const FTransform StartXform = SampleTransformAtDistance(0.0f);
	SetActorTransform(StartXform);
	LastLocation = StartXform.GetLocation();
	UE_LOG(LogSFRace, Log, TEXT("Ghost opponent started on %s / %s (%.0fm)"), *RaceId, *Chosen->Profile, TotalPathLengthMeters);
	return true;
}

void ASFGhostOpponentActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bRacing || PathPointsUnreal.Num() < 2)
	{
		return;
	}

	float Speed = TargetSpeedMps;
	int32 LookIndex = 1;
	for (int32 Index = 1; Index < CumulativeDistancesMeters.Num(); ++Index)
	{
		if (CumulativeDistancesMeters[Index] >= DistanceAlongPathMeters + 20.0f)
		{
			LookIndex = Index;
			break;
		}
		LookIndex = Index;
	}
	const FVector Ahead = PathPointsUnreal[LookIndex] - GetActorLocation();
	const float TurnSharpness = 1.0f - FMath::Clamp(
		FVector::DotProduct(GetActorForwardVector(), Ahead.GetSafeNormal()), 0.0f, 1.0f);
	Speed *= FMath::Lerp(1.0f, TurnSlowdownFactor, TurnSharpness);

	DistanceAlongPathMeters = FMath::Min(TotalPathLengthMeters, DistanceAlongPathMeters + Speed * DeltaSeconds);
	const FTransform Xform = SampleTransformAtDistance(DistanceAlongPathMeters);
	SetActorTransform(Xform);

	if (FVector::DistSquared(LastLocation, Xform.GetLocation()) < 25.0f)
	{
		StuckTimer += DeltaSeconds;
		if (StuckTimer >= StuckResetSeconds)
		{
			// Nudge forward instead of teleporting to destination.
			DistanceAlongPathMeters = FMath::Min(TotalPathLengthMeters, DistanceAlongPathMeters + 8.0f);
			StuckTimer = 0.0f;
			UE_LOG(LogSFRace, Warning, TEXT("Ghost opponent recovered from stuck state"));
		}
	}
	else
	{
		StuckTimer = 0.0f;
	}
	LastLocation = Xform.GetLocation();

	if (DistanceAlongPathMeters >= TotalPathLengthMeters - 0.5f)
	{
		bRacing = false;
		UE_LOG(LogSFRace, Log, TEXT("Ghost opponent reached destination"));
	}
}
