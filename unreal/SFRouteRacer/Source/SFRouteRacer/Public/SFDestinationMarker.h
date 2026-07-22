#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SFDestinationMarker.generated.h"

class UStaticMeshComponent;

/**
 * Visible destination marker with configurable finish radius.
 */
UCLASS()
class SFROUTERACER_API ASFDestinationMarker : public AActor
{
	GENERATED_BODY()

public:
	ASFDestinationMarker();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetDestinationRadiusMeters(float RadiusMeters);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	float GetDestinationRadiusMeters() const { return DestinationRadiusMeters; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	USphereComponent* GetFinishSphere() const { return FinishSphere; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TObjectPtr<USphereComponent> FinishSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	float DestinationRadiusMeters = 25.0f;
};
