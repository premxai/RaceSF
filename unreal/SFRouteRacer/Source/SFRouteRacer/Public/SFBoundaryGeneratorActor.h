#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBoundaryGeneratorActor.generated.h"

class ASFBoundaryBarrierActor;

/**
 * Places visible MVP boundary barriers around the configured export bbox.
 */
UCLASS()
class SFROUTERACER_API ASFBoundaryGeneratorActor : public AActor
{
	GENERATED_BODY()

public:
	ASFBoundaryGeneratorActor();

	UFUNCTION(BlueprintCallable, Category = "SF|Boundary")
	void GenerateFromLoadedMap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "SF|Boundary")
	bool bGenerateOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "SF|Boundary")
	float BarrierHeightCm = 250.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Boundary")
	float InsetMeters = 20.0f;

	UPROPERTY()
	TArray<TObjectPtr<ASFBoundaryBarrierActor>> SpawnedBarriers;
};
