#pragma once

#include "CoreMinimal.h"
#include "SFMapTypes.h"
#include "UObject/Interface.h"
#include "SFRoadGeometryBuilder.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USFRoadGeometryBuilder : public UInterface
{
	GENERATED_BODY()
};

/**
 * Abstraction over how graybox road meshes are generated from centerline data.
 */
class SFGEORUNTIME_API ISFRoadGeometryBuilder
{
	GENERATED_BODY()

public:
	virtual bool BuildRoadMesh(
		UObject* WorldContextObject,
		const FSFRoadEdgeData& Edge,
		class UProceduralMeshComponent* TargetMesh,
		int32 SectionIndex) = 0;
};
