#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBuildingTileActor.generated.h"

class UProceduralMeshComponent;

/**
 * Combines building footprints for one map tile into a single graybox mesh.
 */
UCLASS()
class SFROUTERACER_API ASFBuildingTileActor : public AActor
{
	GENERATED_BODY()

public:
	ASFBuildingTileActor();

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	void BuildFromTileId(const FString& InTileId);

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	void BuildAllLoadedTiles();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Map")
	TObjectPtr<UProceduralMeshComponent> BuildingMesh;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	FString TileId;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	bool bBuildOnBeginPlay = false;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	bool bEnableCollision = false;
};
