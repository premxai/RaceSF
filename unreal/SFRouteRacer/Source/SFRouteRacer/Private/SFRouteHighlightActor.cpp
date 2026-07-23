#include "SFRouteHighlightActor.h"

#include "Engine/GameInstance.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "SFRouteRacer.h"

ASFRouteHighlightActor::ASFRouteHighlightActor()
{
	PrimaryActorTick.bCanEverTick = false;
	HighlightMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("HighlightMesh"));
	SetRootComponent(HighlightMesh);
	HighlightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightMesh->SetCastShadow(false);
}

FLinearColor ASFRouteHighlightActor::ColorForProfile(const FString& Profile)
{
	if (Profile.Equals(TEXT("balanced"), ESearchCase::IgnoreCase))
	{
		return FLinearColor(0.2f, 0.82f, 0.48f);
	}
	if (Profile.Equals(TEXT("scenic"), ESearchCase::IgnoreCase))
	{
		return FLinearColor(0.965f, 0.827f, 0.176f);
	}
	if (Profile.Equals(TEXT("explorer"), ESearchCase::IgnoreCase))
	{
		return FLinearColor(0.647f, 0.427f, 0.886f);
	}
	return FLinearColor(0.125f, 0.851f, 1.0f); // fastest cyan
}

void ASFRouteHighlightActor::HighlightProfileRoute(const TArray<FString>& EdgeIds, const FString& Profile)
{
	SetHighlightedEdgeIds(EdgeIds, ColorForProfile(Profile));
}

void ASFRouteHighlightActor::ClearHighlight()
{
	HighlightMesh->ClearAllMeshSections();
}

void ASFRouteHighlightActor::SetHighlightedEdgeIds(const TArray<FString>& EdgeIds, FLinearColor Color)
{
	ClearHighlight();
	if (EdgeIds.Num() == 0)
	{
		return;
	}

	USFMapDataSubsystem* MapData = GetGameInstance() ? GetGameInstance()->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || !MapData->IsMapLoaded())
	{
		UE_LOG(LogSFRace, Warning, TEXT("Route highlight skipped: map not loaded"));
		return;
	}

	const float HalfWidthCm = HighlightWidthMeters * 50.0f;
	int32 Section = 0;
	for (const FString& EdgeId : EdgeIds)
	{
		FSFRoadEdgeData Edge;
		if (!MapData->FindEdge(EdgeId, Edge) || Edge.PointsLocalMeters.Num() < 2)
		{
			continue;
		}

		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FLinearColor> Colors;
		TArray<FProcMeshTangent> Tangents;

		for (int32 Index = 0; Index < Edge.PointsLocalMeters.Num(); ++Index)
		{
			FVector Point = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index]);
			Point.Z += HeightOffsetCm;
			FVector Forward = FVector::ForwardVector;
			if (Index + 1 < Edge.PointsLocalMeters.Num())
			{
				FVector Next = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index + 1]);
				Next.Z += HeightOffsetCm;
				Forward = (Next - Point).GetSafeNormal2D();
			}
			else if (Index > 0)
			{
				FVector Prev = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index - 1]);
				Prev.Z += HeightOffsetCm;
				Forward = (Point - Prev).GetSafeNormal2D();
			}
			const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
			Vertices.Add(Point - Right * HalfWidthCm);
			Vertices.Add(Point + Right * HalfWidthCm);
			Normals.Add(FVector::UpVector);
			Normals.Add(FVector::UpVector);
			UV0.Add(FVector2D(0.0f, Index));
			UV0.Add(FVector2D(1.0f, Index));
			Colors.Add(Color);
			Colors.Add(Color);
			Tangents.Add(FProcMeshTangent(Forward, false));
			Tangents.Add(FProcMeshTangent(Forward, false));
			if (Index > 0)
			{
				const int32 Base = (Index - 1) * 2;
				Triangles.Append({Base, Base + 1, Base + 2, Base + 1, Base + 3, Base + 2});
			}
		}

		const int32 CreatedSection = Section++;
		HighlightMesh->CreateMeshSection_LinearColor(
			CreatedSection, Vertices, Triangles, Normals, UV0, Colors, Tangents, false);

		// Bright unlit so the route reads in-world and on the orthographic minimap.
		static UMaterialInterface* RouteMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Game/Materials/M_SFStartUnlit.M_SFStartUnlit"));
		if (!RouteMaterial)
		{
			RouteMaterial = LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Game/Materials/M_SFCarUnlit.M_SFCarUnlit"));
		}
		if (RouteMaterial)
		{
			HighlightMesh->SetMaterial(CreatedSection, RouteMaterial);
		}
	}
}
