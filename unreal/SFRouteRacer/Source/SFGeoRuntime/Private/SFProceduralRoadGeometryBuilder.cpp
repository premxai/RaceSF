#include "SFProceduralRoadGeometryBuilder.h"

#include "SFGeoCoordinateLibrary.h"
#include "SFGeoRuntime.h"
#include "ProceduralMeshComponent.h"

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

	UE_LOG(LogSFMapGeneration, Verbose, TEXT("Built road mesh for edge %s (%d verts)"), *Edge.Id, Vertices.Num());
	return true;
}
