#include "SFNavigationSubsystem.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SFAudioSubsystem.h"
#include "SFDestinationMarker.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapDataSubsystem.h"
#include "SFMinimapCaptureActor.h"
#include "SFRaceManager.h"
#include "SFRaceSubsystem.h"
#include "SFRouteHighlightActor.h"
#include "SFRouteRacer.h"
#include "SFVehiclePawn.h"

void USFNavigationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USFNavigationSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TStatId USFNavigationSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USFNavigationSubsystem, STATGROUP_Tickables);
}

void USFNavigationSubsystem::BindActors(
	ASFRouteHighlightActor* Highlight,
	ASFDestinationMarker* Destination,
	ASFMinimapCaptureActor* Minimap)
{
	HighlightActor = Highlight;
	DestinationMarker = Destination;
	MinimapActor = Minimap;
}

void USFNavigationSubsystem::SetDestinationNodeId(const FString& NodeId)
{
	DestinationNodeId = NodeId;
}

void USFNavigationSubsystem::SetActiveRoute(const FSFRoutePath& Route, const FString& Profile)
{
	ApplyRoute(Route, Profile, false);
}

void USFNavigationSubsystem::ApplyRoute(const FSFRoutePath& Route, const FString& Profile, bool bCountAsReroute)
{
	ActiveRoute = Route;
	ActiveProfile = Profile;
	if (ASFRouteHighlightActor* Highlight = HighlightActor.Get())
	{
		Highlight->HighlightProfileRoute(Route.EdgeIds, Profile);
	}

	if (bCountAsReroute)
	{
		bReroutingVisible = true;
		RerouteBannerSeconds = 1.25f;
		if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (USFRaceSubsystem* RaceSubsystem = GameInstance->GetSubsystem<USFRaceSubsystem>())
			{
				if (ASFRaceManager* Manager = RaceSubsystem->GetRaceManager())
				{
					Manager->NotifyReroute();
				}
			}
		}
	}
}

bool USFNavigationSubsystem::ShouldReroute(const FVector& VehicleLocalMeters) const
{
	if (!ActiveRoute.bValid || ActiveRoute.EdgeIds.Num() == 0)
	{
		return false;
	}

	USFMapDataSubsystem* MapData = GetWorld()->GetGameInstance()->GetSubsystem<USFMapDataSubsystem>();
	if (!MapData)
	{
		return false;
	}

	float BestDistance = TNumericLimits<float>::Max();
	for (const FString& EdgeId : ActiveRoute.EdgeIds)
	{
		FSFRoadEdgeData Edge;
		if (!MapData->FindEdge(EdgeId, Edge))
		{
			continue;
		}
		for (int32 Index = 0; Index + 1 < Edge.PointsLocalMeters.Num(); ++Index)
		{
			const FVector Closest = FMath::ClosestPointOnSegment(
				VehicleLocalMeters, Edge.PointsLocalMeters[Index], Edge.PointsLocalMeters[Index + 1]);
			BestDistance = FMath::Min(BestDistance, FVector::Dist(VehicleLocalMeters, Closest));
		}
	}
	return BestDistance > OffRouteThresholdMeters;
}

bool USFNavigationSubsystem::ForceRerouteFromVehicle()
{
	USFRoutingSubsystem* Routing = GetWorld()->GetSubsystem<USFRoutingSubsystem>();
	if (!Routing || DestinationNodeId.IsEmpty())
	{
		return false;
	}

	ASFVehiclePawn* Vehicle = nullptr;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		Vehicle = Cast<ASFVehiclePawn>(PC->GetPawn());
	}
	if (!Vehicle)
	{
		return false;
	}

	const FVector Local = USFGeoCoordinateLibrary::UnrealToLocalMeters(Vehicle->GetActorLocation());
	FString EdgeId;
	FString NodeId;
	if (!Routing->FindNearestDriveableEdge(Local, EdgeId, NodeId))
	{
		return false;
	}

	const FSFRoutePath Path = Routing->FindPath(NodeId, DestinationNodeId);
	if (!Path.bValid)
	{
		UE_LOG(LogSFRace, Warning, TEXT("Reroute failed: no path to destination"));
		return false;
	}

		ApplyRoute(Path, ActiveProfile, true);
	TimeSinceLastReroute = 0.0f;
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (USFAudioSubsystem* Audio = GameInstance->GetSubsystem<USFAudioSubsystem>())
		{
			Audio->PlayRerouteCue();
		}
	}
	return true;
}

void USFNavigationSubsystem::Tick(float DeltaTime)
{
	TimeSinceLastReroute += DeltaTime;
	if (RerouteBannerSeconds > 0.0f)
	{
		RerouteBannerSeconds -= DeltaTime;
		if (RerouteBannerSeconds <= 0.0f)
		{
			bReroutingVisible = false;
		}
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ASFVehiclePawn* Vehicle = PC ? Cast<ASFVehiclePawn>(PC->GetPawn()) : nullptr;
	if (!Vehicle)
	{
		return;
	}

	const FVector VehicleUnreal = Vehicle->GetActorLocation();
	if (ASFDestinationMarker* Destination = DestinationMarker.Get())
	{
		DestinationDistanceMeters = FVector::Dist(VehicleUnreal, Destination->GetActorLocation()) * 0.01f;
	}

	if (USFRaceSubsystem* RaceSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		if (RaceSubsystem->GetRaceState() != ESFRaceState::Racing)
		{
			return;
		}
	}

	const FVector Velocity = Vehicle->GetVelocity();
	if (Velocity.Size() < 50.0f) // stationary threshold ~0.5 m/s
	{
		return;
	}

	if (TimeSinceLastReroute < RerouteCooldownSeconds)
	{
		return;
	}

	const FVector Local = USFGeoCoordinateLibrary::UnrealToLocalMeters(VehicleUnreal);
	if (ShouldReroute(Local))
	{
		ForceRerouteFromVehicle();
	}
}
