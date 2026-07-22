#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFRoadNetworkActor.generated.h"

class UProceduralMeshComponent;
class USFProceduralRoadGeometryBuilder;

/**
 * Builds graybox driveable road meshes from the loaded road graph.
 */
UCLASS()
class SFROUTERACER_API ASFRoadNetworkActor : public AActor
{
	GENERATED_BODY()

public:
	ASFRoadNetworkActor();

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	void RebuildFromMapData();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Map")
	TObjectPtr<UProceduralMeshComponent> RoadMesh;

	UPROPERTY()
	TObjectPtr<USFProceduralRoadGeometryBuilder> GeometryBuilder;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	bool bBuildOnBeginPlay = false;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	int32 MaxEdgesToBuild = 0;
};
