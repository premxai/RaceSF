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
		Colors.Add(FLinearColor(0.12f, 0.12f, 0.13f));
		Colors.Add(FLinearColor(0.12f, 0.12f, 0.13f));
		Tangents.Add(FProcMeshTangent(Forward, false));
		Tangents.Add(FProcMeshTangent(Forward, false));

		if (Index > 0)
		{
			const int32 Base = (Index - 1) * 2;
			Triangles.Append({Base, Base + 1, Base + 2});
			Triangles.Append({Base + 1, Base + 3, Base + 2});
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
		nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (!RoadMaterial)
	{
		RoadMaterial = LoadObject<UMaterialInterface>(
			nullptr, TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
	}
	if (RoadMaterial)
	{
		TargetMesh->SetMaterial(SectionIndex, RoadMaterial);
	}

	// Edge markings for larger roads (visual only, no extra collision sections).
	if (Edge.TotalWidthMeters >= 8.0f)
	{
		TArray<FVector> MarkVerts;
		TArray<int32> MarkTris;
		TArray<FVector> MarkNormals;
		TArray<FVector2D> MarkUV;
		TArray<FLinearColor> MarkColors;
		TArray<FProcMeshTangent> MarkTangents;
		const float MarkHalf = 8.0f;
		for (int32 Index = 0; Index < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector Current = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index])
				+ FVector(0, 0, 2.0f);
			FVector Forward = FVector::ForwardVector;
			if (Index + 1 < Edge.PointsLocalMeters.Num())
			{
				const FVector Next = USFGeoCoordinateLibrary::LocalMetersToUnreal(Edge.PointsLocalMeters[Index + 1]);
				Forward = (Next - Current).GetSafeNormal2D();
			}
			const FVector Right = FVector::CrossProduct(FVector::UpVector, Forward).GetSafeNormal();
			const FVector LeftEdge = Current - Right * (HalfWidthCm - 12.0f);
			const FVector RightEdge = Current + Right * (HalfWidthCm - 12.0f);
			for (const FVector& Center : {LeftEdge, RightEdge})
			{
				MarkVerts.Add(Center - Right * MarkHalf);
				MarkVerts.Add(Center + Right * MarkHalf);
				MarkNormals.Add(FVector::UpVector);
				MarkNormals.Add(FVector::UpVector);
				MarkUV.Add(FVector2D(0, Index));
				MarkUV.Add(FVector2D(1, Index));
				MarkColors.Add(FLinearColor(0.85f, 0.85f, 0.8f));
				MarkColors.Add(FLinearColor(0.85f, 0.85f, 0.8f));
				MarkTangents.Add(FProcMeshTangent(Forward, false));
				MarkTangents.Add(FProcMeshTangent(Forward, false));
			}
		}
		// Build left then right marking strips.
		const int32 Points = Edge.PointsLocalMeters.Num();
		for (int32 Side = 0; Side < 2; ++Side)
		{
			for (int32 Index = 0; Index + 1 < Points; ++Index)
			{
				const int32 Base = (Side * Points + Index) * 2;
				MarkTris.Append({Base, Base + 1, Base + 2, Base + 1, Base + 3, Base + 2});
			}
		}
		TargetMesh->CreateMeshSection_LinearColor(
			SectionIndex + 100000,
			MarkVerts,
			MarkTris,
			MarkNormals,
			MarkUV,
			MarkColors,
			MarkTangents,
			false);
	}

	UE_LOG(LogSFMapGeneration, Verbose, TEXT("Built road mesh for edge %s (%d verts)"), *Edge.Id, Vertices.Num());
	return true;
}
