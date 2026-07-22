#include "SFRoutingSubsystem.h"

#include "Algo/Reverse.h"
#include "Engine/GameInstance.h"
#include "SFGeoRuntime.h"
#include "SFMapDataSubsystem.h"

void USFRoutingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USFRoutingSubsystem::RebuildFromMapData()
{
	Adjacency.Reset();
	NodePositions.Reset();
	DriveableEdges.Reset();

	UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || !MapData->IsMapLoaded())
	{
		UE_LOG(LogSFRouting, Warning, TEXT("Cannot rebuild routing graph: map data is not loaded"));
		return;
	}

	for (const FSFRoadNodeData& Node : MapData->GetNodes())
	{
		NodePositions.Add(Node.Id, FVector(Node.XMeters, Node.YMeters, Node.ElevationMeters));
	}

	for (const FSFRoadEdgeData& Edge : MapData->GetEdges())
	{
		if (!Edge.bDriveable)
		{
			continue;
		}
		DriveableEdges.Add(Edge);
		Adjacency.FindOrAdd(Edge.FromNodeId).Add({Edge.ToNodeId, Edge.Id, Edge.TravelTimeSeconds, Edge.LengthMeters});
	}

	UE_LOG(LogSFRouting, Log, TEXT("Routing graph rebuilt with %d driveable edges"), DriveableEdges.Num());
}

FSFRoutePath USFRoutingSubsystem::FindPath(const FString& StartNodeId, const FString& GoalNodeId) const
{
	FSFRoutePath Result;
	if (!Adjacency.Contains(StartNodeId) || !NodePositions.Contains(GoalNodeId))
	{
		return Result;
	}

	TMap<FString, float> BestCost;
	TMap<FString, FString> CameFromNode;
	TMap<FString, FString> CameFromEdge;
	TSet<FString> Visited;

	struct FQueueItem
	{
		FString NodeId;
		float Cost = 0.0f;
		bool operator<(const FQueueItem& Other) const { return Cost > Other.Cost; }
	};

	TArray<FQueueItem> Open;
	Open.HeapPush({StartNodeId, 0.0f});
	BestCost.Add(StartNodeId, 0.0f);

	while (Open.Num() > 0)
	{
		FQueueItem Current;
		Open.HeapPop(Current);
		if (Visited.Contains(Current.NodeId))
		{
			continue;
		}
		Visited.Add(Current.NodeId);
		if (Current.NodeId == GoalNodeId)
		{
			break;
		}

		const TArray<FOutgoingEdge>* Outgoing = Adjacency.Find(Current.NodeId);
		if (!Outgoing)
		{
			continue;
		}

		for (const FOutgoingEdge& Edge : *Outgoing)
		{
			const float NewCost = Current.Cost + Edge.TravelTimeSeconds;
			if (const float* Existing = BestCost.Find(Edge.ToNodeId); Existing && NewCost >= *Existing)
			{
				continue;
			}
			BestCost.Add(Edge.ToNodeId, NewCost);
			CameFromNode.Add(Edge.ToNodeId, Current.NodeId);
			CameFromEdge.Add(Edge.ToNodeId, Edge.EdgeId);
			Open.HeapPush({Edge.ToNodeId, NewCost});
		}
	}

	if (!CameFromNode.Contains(GoalNodeId) && StartNodeId != GoalNodeId)
	{
		return Result;
	}

	TArray<FString> ReverseNodes;
	TArray<FString> ReverseEdges;
	FString Cursor = GoalNodeId;
	ReverseNodes.Add(Cursor);
	while (Cursor != StartNodeId)
	{
		const FString* Previous = CameFromNode.Find(Cursor);
		const FString* EdgeId = CameFromEdge.Find(Cursor);
		if (!Previous || !EdgeId)
		{
			return FSFRoutePath();
		}
		ReverseEdges.Add(*EdgeId);
		Cursor = *Previous;
		ReverseNodes.Add(Cursor);
	}

	Algo::Reverse(ReverseNodes);
	Algo::Reverse(ReverseEdges);
	Result.NodeIds = MoveTemp(ReverseNodes);
	Result.EdgeIds = MoveTemp(ReverseEdges);

	for (const FString& EdgeId : Result.EdgeIds)
	{
		for (const FSFRoadEdgeData& Edge : DriveableEdges)
		{
			if (Edge.Id == EdgeId)
			{
				Result.DistanceMeters += Edge.LengthMeters;
				Result.TravelTimeSeconds += Edge.TravelTimeSeconds;
				break;
			}
		}
	}

	Result.bValid = true;
	return Result;
}

bool USFRoutingSubsystem::FindNearestDriveableEdge(const FVector& LocalMeters, FString& OutEdgeId, FString& OutNodeId) const
{
	float BestDistanceSq = TNumericLimits<float>::Max();
	bool bFound = false;

	for (const FSFRoadEdgeData& Edge : DriveableEdges)
	{
		for (int32 Index = 0; Index + 1 < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector Closest = FMath::ClosestPointOnSegment(
				LocalMeters,
				Edge.PointsLocalMeters[Index],
				Edge.PointsLocalMeters[Index + 1]);
			const float DistanceSq = FVector::DistSquared(LocalMeters, Closest);
			if (DistanceSq < BestDistanceSq)
			{
				BestDistanceSq = DistanceSq;
				OutEdgeId = Edge.Id;
				const float DistToStart = FVector::DistSquared(Closest, Edge.PointsLocalMeters[0]);
				const float DistToEnd = FVector::DistSquared(Closest, Edge.PointsLocalMeters.Last());
				OutNodeId = DistToStart <= DistToEnd ? Edge.FromNodeId : Edge.ToNodeId;
				bFound = true;
			}
		}
	}

	return bFound;
}
