#include "SFProceduralRoadGeometryBuilder.h"

#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFGeoRuntime.h"

bool USFProceduralRoadGeometryBuilder::BuildRoadMesh(
	UObject* WorldContextObject,
	const FSFRoadEdgeData& Edge,
	UProceduralMeshComponent* TargetMesh,
	int32 SectionIndex)
{
	if (!TargetMesh || Edge.PointsLocalMeters.Num() < 2 || !Edge.bDriveable)
	{
		return false;
	}

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	const float HalfWidthCm = FMath::Max(50.0f, Edge.TotalWidthMeters * 50.0f);

	for (int32 Index = 0; Index < Edge.PointsLocalMeters.Num(); ++Index)
	{
		const FVector Current = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index]);
		FVector Forward = FVector::ForwardVector;
		if (Index + 1 < Edge.PointsLocalMeters.Num())
		{
			const FVector Next = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index + 1]);
			Forward = (Next - Current).GetSafeNormal2D();
		}
		else if (Index > 0)
		{
			const FVector Prev = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index - 1]);
			Forward = (Current - Prev).GetSafeNormal2D();
		}

		const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
		const FVector LeftPoint = Current - Right * HalfWidthCm;
		const FVector RightPoint = Current + Right * HalfWidthCm;

		Vertices.Add(LeftPoint);
		Vertices.Add(RightPoint);
		Normals.Add(FVector::UpVector);
		Normals.Add(FVector::UpVector);
		UV0.Add(FVector2D(0.0f, static_cast<float>(Index)));
		UV0.Add(FVector2D(1.0f, static_cast<float>(Index)));
		// Bright gray so lit/unlit materials stay visible.
		Colors.Add(FLinearColor(0.55f, 0.55f, 0.58f));
		Colors.Add(FLinearColor(0.55f, 0.55f, 0.58f));
		Tangents.Add(FProcMeshTangent(Forward, false));
		Tangents.Add(FProcMeshTangent(Forward, false));

		if (Index > 0)
		{
			const int32 Base = (Index - 1) * 2;
			Triangles.Append({Base, Base + 1, Base + 2});
			Triangles.Append({Base + 1, Base + 3, Base + 2});
		}
	}

	// Optional center stripe baked into the same section (never use huge section indices —
	// ProceduralMesh densifies its section array up to SectionIndex).
	if (Edge.TotalWidthMeters >= 8.0f && Edge.PointsLocalMeters.Num() >= 2)
	{
		const int32 StripeStart = Vertices.Num();
		const float StripeHalf = 12.0f;
		for (int32 Index = 0; Index < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector Current = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index])
				+ FVector(0.0f, 0.0f, 3.0f);
			FVector Forward = FVector::ForwardVector;
			if (Index + 1 < Edge.PointsLocalMeters.Num())
			{
				const FVector Next = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index + 1]);
				Forward = (Next - Current).GetSafeNormal2D();
			}
			else if (Index > 0)
			{
				const FVector Prev = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index - 1]);
				Forward = (Current - Prev).GetSafeNormal2D();
			}
			const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
			Vertices.Add(Current - Right * StripeHalf);
			Vertices.Add(Current + Right * StripeHalf);
			Normals.Add(FVector::UpVector);
			Normals.Add(FVector::UpVector);
			UV0.Add(FVector2D(0.0f, static_cast<float>(Index)));
			UV0.Add(FVector2D(1.0f, static_cast<float>(Index)));
			Colors.Add(FLinearColor(0.95f, 0.9f, 0.2f));
			Colors.Add(FLinearColor(0.95f, 0.9f, 0.2f));
			Tangents.Add(FProcMeshTangent(Forward, false));
			Tangents.Add(FProcMeshTangent(Forward, false));
			if (Index > 0)
			{
				const int32 Base = StripeStart + (Index - 1) * 2;
				Triangles.Append({Base, Base + 1, Base + 2});
				Triangles.Append({Base + 1, Base + 3, Base + 2});
			}
		}
	}

	TargetMesh->CreateMeshSection_LinearColor(
		SectionIndex,
		Vertices,
		Triangles,
		Normals,
		UV0,
		Colors,
		Tangents,
		true);

	static UMaterialInterface* RoadMaterial = LoadObject<UMaterialInterface>(
		nullptr, TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
	if (!RoadMaterial)
	{
		RoadMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
	if (RoadMaterial)
	{
		TargetMesh->SetMaterial(SectionIndex, RoadMaterial);
	}

	UE_LOG(LogSFMapGeneration, Verbose, TEXT("Built road mesh for edge %s (%d verts)"), *Edge.Id, Vertices.Num());
	return true;
}
