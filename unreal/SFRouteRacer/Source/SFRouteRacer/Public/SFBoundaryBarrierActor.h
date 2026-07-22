#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBoundaryBarrierActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

/**
 * Visible MVP boundary barrier / construction closure (not an invisible mid-street wall).
 */
UCLASS()
class SFROUTERACER_API ASFBoundaryBarrierActor : public AActor
{
	GENERATED_BODY()

public:
	ASFBoundaryBarrierActor();

	UFUNCTION(BlueprintCallable, Category = "SF|Boundary")
	void Configure(const FVector& Location, const FRotator& Rotation, float LengthCm, float HeightCm);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Boundary")
	TObjectPtr<UBoxComponent> BlockingVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Boundary")
	TObjectPtr<UStaticMeshComponent> BarrierMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Boundary")
	FString SignageText = TEXT("Area not available in MVP");
};
