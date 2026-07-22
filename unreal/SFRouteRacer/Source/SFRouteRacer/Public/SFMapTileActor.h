#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFMapTileActor.generated.h"

/**
 * Placeholder World Partition-aligned tile actor for a single export tile.
 */
UCLASS()
class SFROUTERACER_API ASFMapTileActor : public AActor
{
	GENERATED_BODY()

public:
	ASFMapTileActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SF|Map")
	FString TileId;
};
