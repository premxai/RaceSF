#pragma once

#include "CoreMinimal.h"
#include "SFMapTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFRoutingSubsystem.generated.h"

USTRUCT(BlueprintType)
struct SFGEORUNTIME_API FSFRoutePath
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SF|Route")
	TArray<FString> NodeIds;

	UPROPERTY(BlueprintReadOnly, Category = "SF|Route")
	TArray<FString> EdgeIds;

	UPROPERTY(BlueprintReadOnly, Category = "SF|Route")
	float DistanceMeters = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SF|Route")
	float TravelTimeSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SF|Route")
	bool bValid = false;
};

/**
 * Runtime Dijkstra routing over the driveable directed graph.
 */
UCLASS()
class SFGEORUNTIME_API USFRoutingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Routing")
	void RebuildFromMapData();

	UFUNCTION(BlueprintCallable, Category = "SF|Routing")
	FSFRoutePath FindPath(const FString& StartNodeId, const FString& GoalNodeId) const;

	UFUNCTION(BlueprintCallable, Category = "SF|Routing")
	bool FindNearestDriveableEdge(const FVector& LocalMeters, FString& OutEdgeId, FString& OutNodeId) const;

	UFUNCTION(BlueprintPure, Category = "SF|Routing")
	bool HasGraph() const { return Adjacency.Num() > 0; }

private:
	struct FOutgoingEdge
	{
		FString ToNodeId;
		FString EdgeId;
		float TravelTimeSeconds = 0.0f;
		float LengthMeters = 0.0f;
	};

	TMap<FString, TArray<FOutgoingEdge>> Adjacency;
	TMap<FString, FVector> NodePositions;
	TArray<FSFRoadEdgeData> DriveableEdges;
};
