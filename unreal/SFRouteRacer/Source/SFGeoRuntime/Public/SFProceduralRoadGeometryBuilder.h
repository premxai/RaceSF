#pragma once

#include "CoreMinimal.h"
#include "SFRoadGeometryBuilder.h"
#include "UObject/Object.h"
#include "SFProceduralRoadGeometryBuilder.generated.h"

/**
 * Stable MVP road builder using Procedural Mesh strips from centerlines.
 */
UCLASS(BlueprintType)
class SFGEORUNTIME_API USFProceduralRoadGeometryBuilder : public UObject, public ISFRoadGeometryBuilder
{
	GENERATED_BODY()

public:
	virtual bool BuildRoadMesh(
		UObject* WorldContextObject,
		const FSFRoadEdgeData& Edge,
		class UProceduralMeshComponent* TargetMesh,
		int32 SectionIndex) override;
};
