#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFGhostOpponentActor.generated.h"

class UStaticMeshComponent;

/**
 * Intermediate Milestone opponent: replays a suggested route as a ghost vehicle.
 * Physical AI can replace this later without changing the race destination contract.
 */
UCLASS()
class SFROUTERACER_API ASFGhostOpponentActor : public AActor
{
	GENERATED_BODY()

public:
	ASFGhostOpponentActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "SF|AI")
	bool StartGhostForRace(const FString& RaceId, const FString& Profile = TEXT("scenic"));

	UFUNCTION(BlueprintCallable, Category = "SF|AI")
	void StopGhost();

	UFUNCTION(BlueprintPure, Category = "SF|AI")
	bool IsRacing() const { return bRacing; }

	UFUNCTION(BlueprintPure, Category = "SF|AI")
	float GetProgressAlpha() const;

protected:
	virtual void BeginPlay() override;

	void RebuildPathSamples(const TArray<FString>& EdgeIds);
	FTransform SampleTransformAtDistance(float DistanceMeters) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|AI")
	TObjectPtr<UStaticMeshComponent> GhostMesh;

	UPROPERTY(EditAnywhere, Category = "SF|AI")
	float TargetSpeedMps = 16.0f;

	UPROPERTY(EditAnywhere, Category = "SF|AI")
	float TurnSlowdownFactor = 0.65f;

	UPROPERTY(EditAnywhere, Category = "SF|AI")
	float StuckResetSeconds = 4.0f;

	bool bRacing = false;
	float DistanceAlongPathMeters = 0.0f;
	float TotalPathLengthMeters = 0.0f;
	float StuckTimer = 0.0f;
	FVector LastLocation = FVector::ZeroVector;

	UPROPERTY()
	TArray<FVector> PathPointsUnreal;

	UPROPERTY()
	TArray<float> CumulativeDistancesMeters;
};
