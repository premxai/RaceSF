#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFRaceCheckpoint.generated.h"

/**
 * Placeholder checkpoint actor reserved for later race loop work.
 */
UCLASS()
class SFROUTERACER_API ASFRaceCheckpoint : public AActor
{
	GENERATED_BODY()

public:
	ASFRaceCheckpoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SF|Race")
	FString CheckpointId;
};
