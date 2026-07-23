#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SFDestinationMarker.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ESFLandmarkMarkerStyle : uint8
{
	Finish UMETA(DisplayName = "Finish"),
	Start UMETA(DisplayName = "Start")
};

/**
 * Visible landmark marker with configurable finish radius.
 */
UCLASS()
class SFROUTERACER_API ASFDestinationMarker : public AActor
{
	GENERATED_BODY()

public:
	ASFDestinationMarker();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetDestinationRadiusMeters(float RadiusMeters);

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetMarkerStyle(ESFLandmarkMarkerStyle Style);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	float GetDestinationRadiusMeters() const { return DestinationRadiusMeters; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	USphereComponent* GetFinishSphere() const { return FinishSphere; }

protected:
	virtual void BeginPlay() override;

	void ApplyVisuals();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TObjectPtr<USphereComponent> FinishSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TObjectPtr<UStaticMeshComponent> PoleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	TObjectPtr<UStaticMeshComponent> MarkerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	float DestinationRadiusMeters = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	ESFLandmarkMarkerStyle MarkerStyle = ESFLandmarkMarkerStyle::Finish;
};
