#include "SFBuildingTileActor.h"

#include "Engine/GameInstance.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "SFRouteRacer.h"

ASFBuildingTileActor::ASFBuildingTileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	BuildingMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("BuildingMesh"));
	SetRootComponent(BuildingMesh);
	BuildingMesh->bUseAsyncCooking = true;
}

void ASFBuildingTileActor::BeginPlay()
{
	Super::BeginPlay();
	if (bBuildOnBeginPlay)
	{
		if (!TileId.IsEmpty())
		{
			BuildFromTileId(TileId);
		}
		else
		{
			BuildAllLoadedTiles();
		}
	}
}

void ASFBuildingTileActor::BuildFromTileId(const FString& InTileId)
{
	TileId = InTileId;
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Error, TEXT("Building tile cannot build: map data unavailable"));
		return;
	}

	BuildingMesh->ClearAllMeshSections();
	BuildingMesh->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

	int32 Section = 0;
	for (const FSFMapTileData& Tile : MapData->GetTiles())
	{
		if (Tile.TileId != TileId)
		{
			continue;
		}

		for (const FSFBuildingData& Building : Tile.Buildings)
		{
			if (Building.OuterRingLocalMeters.Num() < 3)
			{
				continue;
			}

			TArray<FVector> Vertices;
			TArray<int32> Triangles;
			TArray<FVector> Normals;
			TArray<FVector2D> UV0;
			TArray<FLinearColor> Colors;
			TArray<FProcMeshTangent> Tangents;

			const float HeightCm = FMath::Max(100.0f, Building.HeightMeters * 100.0f);
			const int32 RingCount = Building.OuterRingLocalMeters.Num();
			// Drop duplicate closing vertex if present.
			const int32 PointCount = (RingCount > 1
				&& Building.OuterRingLocalMeters[0].Equals(Building.OuterRingLocalMeters.Last(), 0.01f))
				? RingCount - 1
				: RingCount;
			if (PointCount < 3)
			{
				continue;
			}

			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				const FVector2D Local = Building.OuterRingLocalMeters[Index];
				const FVector Base = USFGeoCoordinateLibrary::Point2DLocalToUnreal(Local.X, Local.Y, 0.0);
				const FVector Top = Base + FVector(0.0f, 0.0f, HeightCm);
				Vertices.Add(Base);
				Vertices.Add(Top);
				Normals.Add(FVector::UpVector);
				Normals.Add(FVector::UpVector);
				UV0.Add(FVector2D(0.0f, 0.0f));
				UV0.Add(FVector2D(0.0f, 1.0f));
				Colors.Add(FLinearColor(0.55f, 0.52f, 0.48f));
				Colors.Add(FLinearColor(0.62f, 0.58f, 0.52f));
				Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
				Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
			}

			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				const int32 Next = (Index + 1) % PointCount;
				const int32 BaseA = Index * 2;
				const int32 BaseB = Next * 2;
				Triangles.Append({BaseA, BaseB, BaseA + 1});
				Triangles.Append({BaseB, BaseB + 1, BaseA + 1});
			}

			// Simple fan roof.
			const int32 RoofStart = Vertices.Num();
			FVector Centroid = FVector::ZeroVector;
			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				Centroid += Vertices[Index * 2 + 1];
			}
			Centroid /= static_cast<float>(PointCount);
			Vertices.Add(Centroid);
			Normals.Add(FVector::UpVector);
			UV0.Add(FVector2D(0.5f, 0.5f));
			Colors.Add(FLinearColor(0.45f, 0.42f, 0.40f));
			Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				const int32 Next = (Index + 1) % PointCount;
				Triangles.Append({RoofStart, Index * 2 + 1, Next * 2 + 1});
			}

			BuildingMesh->CreateMeshSection_LinearColor(Section, Vertices, Triangles, Normals, UV0, Colors, Tangents, bEnableCollision);
			static UMaterialInterface* BuildingMaterial = LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
			if (!BuildingMaterial)
			{
				BuildingMaterial = LoadObject<UMaterialInterface>(
					nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
			}
			if (BuildingMaterial)
			{
				BuildingMesh->SetMaterial(Section, BuildingMaterial);
			}
			++Section;
		}
	}

	UE_LOG(LogSFRace, Log, TEXT("Built buildings for tile %s (%d sections)"), *TileId, Section);
}

void ASFBuildingTileActor::BuildAllLoadedTiles()
{
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Error, TEXT("Cannot build all tiles: map data unavailable"));
		return;
	}

	BuildingMesh->ClearAllMeshSections();
	BuildingMesh->SetCollisionEnabled(bEnableCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);

	// One actor can host many tile sections for the graybox MVP.
	int32 Section = 0;
	for (const FSFMapTileData& Tile : MapData->GetTiles())
	{
		for (const FSFBuildingData& Building : Tile.Buildings)
		{
			if (Building.OuterRingLocalMeters.Num() < 3)
			{
				continue;
			}

			TArray<FVector> Vertices;
			TArray<int32> Triangles;
			TArray<FVector> Normals;
			TArray<FVector2D> UV0;
			TArray<FLinearColor> Colors;
			TArray<FProcMeshTangent> Tangents;

			const float HeightCm = FMath::Max(100.0f, Building.HeightMeters * 100.0f);
			const int32 RingCount = Building.OuterRingLocalMeters.Num();
			const int32 PointCount = (RingCount > 1
				&& Building.OuterRingLocalMeters[0].Equals(Building.OuterRingLocalMeters.Last(), 0.01f))
				? RingCount - 1
				: RingCount;
			if (PointCount < 3)
			{
				continue;
			}

			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				const FVector2D Local = Building.OuterRingLocalMeters[Index];
				const FVector Base = USFGeoCoordinateLibrary::Point2DLocalToUnreal(Local.X, Local.Y, 0.0);
				Vertices.Add(Base);
				Vertices.Add(Base + FVector(0.0f, 0.0f, HeightCm));
				Normals.Add(FVector::UpVector);
				Normals.Add(FVector::UpVector);
				UV0.Add(FVector2D(0.0f, 0.0f));
				UV0.Add(FVector2D(0.0f, 1.0f));
				Colors.Add(FLinearColor(0.55f, 0.52f, 0.48f));
				Colors.Add(FLinearColor(0.62f, 0.58f, 0.52f));
				Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
				Tangents.Add(FProcMeshTangent(FVector::ForwardVector, false));
			}

			for (int32 Index = 0; Index < PointCount; ++Index)
			{
				const int32 Next = (Index + 1) % PointCount;
				const int32 BaseA = Index * 2;
				const int32 BaseB = Next * 2;
				Triangles.Append({BaseA, BaseB, BaseA + 1});
				Triangles.Append({BaseB, BaseB + 1, BaseA + 1});
			}

			BuildingMesh->CreateMeshSection_LinearColor(Section, Vertices, Triangles, Normals, UV0, Colors, Tangents, bEnableCollision);
			static UMaterialInterface* BuildingMaterial = LoadObject<UMaterialInterface>(
				nullptr, TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
			if (!BuildingMaterial)
			{
				BuildingMaterial = LoadObject<UMaterialInterface>(
					nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
			}
			if (BuildingMaterial)
			{
				BuildingMesh->SetMaterial(Section, BuildingMaterial);
			}
			++Section;
		}
	}

	UE_LOG(LogSFRace, Log, TEXT("Built combined building graybox with %d sections"), Section);
}
