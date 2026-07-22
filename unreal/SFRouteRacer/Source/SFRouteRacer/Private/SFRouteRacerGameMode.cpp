#include "SFRouteRacerGameMode.h"

#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "Engine/SkyLight.h"
#include "Kismet/GameplayStatics.h"
#include "SFBoundaryGeneratorActor.h"
#include "SFBuildingTileActor.h"
#include "SFDebugDrawActor.h"
#include "SFDestinationMarker.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFGhostOpponentActor.h"
#include "SFMapDataSubsystem.h"
#include "SFMinimapCaptureActor.h"
#include "SFNavigationSubsystem.h"
#include "SFRaceManager.h"
#include "SFRaceSubsystem.h"
#include "SFRoadNetworkActor.h"
#include "SFRouteHighlightActor.h"
#include "SFRouteRacer.h"
#include "SFRouteRacerPlayerController.h"
#include "SFRoutingSubsystem.h"
#include "SFVehiclePawn.h"

ASFRouteRacerGameMode::ASFRouteRacerGameMode()
{
	DefaultPawnClass = ASFVehiclePawn::StaticClass();
	PlayerControllerClass = ASFRouteRacerPlayerController::StaticClass();
	VehicleClass = ASFVehiclePawn::StaticClass();
}

void ASFRouteRacerGameMode::StartPlay()
{
	Super::StartPlay();
	EnsureWorldLighting();
	if (bSpawnMapActorsOnStart)
	{
		BootstrapGrayboxWorld();
	}
	if (bAutoStartDefaultRace)
	{
		BeginDefaultRace();
	}
}

void ASFRouteRacerGameMode::EnsureWorldLighting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<AActor*> Existing;
	UGameplayStatics::GetAllActorsOfClass(World, ADirectionalLight::StaticClass(), Existing);
	if (Existing.Num() == 0)
	{
		ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
			ADirectionalLight::StaticClass(),
			FVector::ZeroVector,
			FRotator(-50.0f, 35.0f, 0.0f),
			SpawnParams);
		if (Sun && Sun->GetLightComponent())
		{
			Sun->GetLightComponent()->SetIntensity(12.0f);
			Sun->GetLightComponent()->SetLightColor(FLinearColor(1.0f, 0.96f, 0.9f));
			Sun->GetLightComponent()->SetMobility(EComponentMobility::Movable);
			Sun->GetLightComponent()->SetCastShadows(true);
		}
	}

	Existing.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, ASkyLight::StaticClass(), Existing);
	if (Existing.Num() == 0)
	{
		ASkyLight* Sky = World->SpawnActor<ASkyLight>(ASkyLight::StaticClass(), FTransform::Identity, SpawnParams);
		if (Sky && Sky->GetLightComponent())
		{
			Sky->GetLightComponent()->SetIntensity(1.5f);
			Sky->GetLightComponent()->bRealTimeCapture = true;
			Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);
			Sky->GetLightComponent()->RecaptureSky();
		}
	}

	Existing.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, ASkyAtmosphere::StaticClass(), Existing);
	if (Existing.Num() == 0)
	{
		World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FTransform::Identity, SpawnParams);
	}

	Existing.Reset();
	UGameplayStatics::GetAllActorsOfClass(World, AExponentialHeightFog::StaticClass(), Existing);
	if (Existing.Num() == 0)
	{
		AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(
			AExponentialHeightFog::StaticClass(), FTransform::Identity, SpawnParams);
		if (Fog && Fog->GetComponent())
		{
			Fog->GetComponent()->SetFogDensity(0.0015f);
			Fog->GetComponent()->SetFogHeightFalloff(0.15f);
		}
	}

	UE_LOG(LogSFRace, Log, TEXT("World lighting ensured for graybox play"));
}

bool ASFRouteRacerGameMode::BootstrapGrayboxWorld()
{
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData)
	{
		UE_LOG(LogSFRace, Error, TEXT("Bootstrap failed: map subsystem missing"));
		return false;
	}

	if (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport())
	{
		UE_LOG(LogSFRace, Error, TEXT("Bootstrap failed: %s"), *MapData->GetLastError());
		return false;
	}

	if (USFRoutingSubsystem* Routing = GetWorld()->GetSubsystem<USFRoutingSubsystem>())
	{
		Routing->RebuildFromMapData();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	RoadNetworkActor = GetWorld()->SpawnActor<ASFRoadNetworkActor>(
		ASFRoadNetworkActor::StaticClass(), FTransform::Identity, SpawnParams);
	BuildingTileActor = GetWorld()->SpawnActor<ASFBuildingTileActor>(
		ASFBuildingTileActor::StaticClass(), FTransform::Identity, SpawnParams);
	DestinationMarker = GetWorld()->SpawnActor<ASFDestinationMarker>(
		ASFDestinationMarker::StaticClass(), FTransform::Identity, SpawnParams);
	RaceManager = GetWorld()->SpawnActor<ASFRaceManager>(
		ASFRaceManager::StaticClass(), FTransform::Identity, SpawnParams);
	RouteHighlightActor = GetWorld()->SpawnActor<ASFRouteHighlightActor>(
		ASFRouteHighlightActor::StaticClass(), FTransform::Identity, SpawnParams);
	MinimapCaptureActor = GetWorld()->SpawnActor<ASFMinimapCaptureActor>(
		ASFMinimapCaptureActor::StaticClass(), FTransform::Identity, SpawnParams);
	BoundaryGenerator = GetWorld()->SpawnActor<ASFBoundaryGeneratorActor>(
		ASFBoundaryGeneratorActor::StaticClass(), FTransform::Identity, SpawnParams);
	DebugDrawActor = GetWorld()->SpawnActor<ASFDebugDrawActor>(
		ASFDebugDrawActor::StaticClass(), FTransform::Identity, SpawnParams);
	if (bSpawnGhostOpponent)
	{
		GhostOpponent = GetWorld()->SpawnActor<ASFGhostOpponentActor>(
			ASFGhostOpponentActor::StaticClass(), FTransform::Identity, SpawnParams);
	}

	if (RoadNetworkActor)
	{
		RoadNetworkActor->RebuildFromMapData();
	}
	if (BuildingTileActor)
	{
		BuildingTileActor->BuildAllLoadedTiles();
	}
	if (RaceManager)
	{
		RaceManager->BindDestinationMarker(DestinationMarker);
		if (USFRaceSubsystem* RaceSubsystem = GameInstance->GetSubsystem<USFRaceSubsystem>())
		{
			RaceSubsystem->RegisterRaceManager(RaceManager);
			RaceSubsystem->SetRaceState(ESFRaceState::MapLoading);
		}
	}
	if (USFNavigationSubsystem* Navigation = GetWorld()->GetSubsystem<USFNavigationSubsystem>())
	{
		Navigation->BindActors(RouteHighlightActor, DestinationMarker, MinimapCaptureActor);
	}
	if (DebugDrawActor)
	{
		DebugDrawActor->SetDebugEnabled(bEnableDebugDraw);
	}

	FSFRaceDefinitionData Race;
	FSFLandmarkData Destination;
	if (MapData->FindRace(DefaultRaceId, Race)
		&& MapData->FindLandmark(Race.DestinationLandmarkId, Destination)
		&& Destination.bHasSpawn)
	{
		const FVector DestLocation = USFGeoCoordinateLibrary::Point2DLocalToUnreal(
			Destination.Spawn.XMeters, Destination.Spawn.YMeters, 0.0);
		if (DestinationMarker)
		{
			DestinationMarker->SetActorLocation(DestLocation);
			DestinationMarker->SetDestinationRadiusMeters(25.0f);
		}
		if (USFNavigationSubsystem* Navigation = GetWorld()->GetSubsystem<USFNavigationSubsystem>())
		{
			Navigation->SetDestinationNodeId(Destination.Spawn.NodeId);
		}
	}

	FSFLandmarkData Start;
	if (MapData->FindRace(DefaultRaceId, Race)
		&& MapData->FindLandmark(Race.StartLandmarkId, Start)
		&& Start.bHasSpawn)
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				const FVector StartLocation = USFGeoCoordinateLibrary::Point2DLocalToUnreal(
					Start.Spawn.XMeters, Start.Spawn.YMeters, 2.0);
				const FRotator StartRotation(0.0f, Start.Spawn.HeadingDegrees, 0.0f);
				Pawn->SetActorLocationAndRotation(
					StartLocation, StartRotation, false, nullptr, ETeleportType::TeleportPhysics);
				if (ASFVehiclePawn* Vehicle = Cast<ASFVehiclePawn>(Pawn))
				{
					Vehicle->CaptureSpawnTransform();
				}
			}
			if (MinimapCaptureActor)
			{
				MinimapCaptureActor->SetFollowTarget(PC->GetPawn());
			}
		}
	}

	UE_LOG(LogSFRace, Log, TEXT("Graybox world bootstrap complete for race %s"), *DefaultRaceId);
	return true;
}

bool ASFRouteRacerGameMode::BeginDefaultRace()
{
	if (!RaceManager)
	{
		UE_LOG(LogSFRace, Error, TEXT("BeginDefaultRace failed: race manager missing"));
		return false;
	}

	if (USFRaceSubsystem* RaceSubsystem = GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		RaceSubsystem->SetActiveRaceId(DefaultRaceId);
		RaceSubsystem->SetSelectedRouteProfile(TEXT("fastest"));
	}

	const bool bStarted = RaceManager->StartRace(DefaultRaceId, TEXT("fastest"));
	if (bStarted && GhostOpponent)
	{
		GhostOpponent->StartGhostForRace(DefaultRaceId, GhostRouteProfile);
	}
	return bStarted;
}
