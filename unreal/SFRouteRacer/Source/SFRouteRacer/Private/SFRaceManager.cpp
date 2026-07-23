#include "SFRaceManager.h"

#include "Components/SphereComponent.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SFAudioSubsystem.h"
#include "SFDestinationMarker.h"
#include "SFMapDataSubsystem.h"
#include "SFMinimapCaptureActor.h"
#include "SFNavigationSubsystem.h"
#include "SFRaceSubsystem.h"
#include "SFRouteRacer.h"
#include "SFRouteRacerPlayerController.h"
#include "SFRoutingSubsystem.h"
#include "SFSaveGame.h"
#include "SFVehiclePawn.h"

ASFRaceManager::ASFRaceManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASFRaceManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASFRaceManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	switch (RaceState)
	{
	case ESFRaceState::Countdown:
		AdvanceCountdown(DeltaSeconds);
		break;
	case ESFRaceState::Racing:
		UpdateRacing(DeltaSeconds);
		break;
	default:
		break;
	}

	PushHud();
}

void ASFRaceManager::SyncSubsystemState(ESFRaceState NewState) const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USFRaceSubsystem* RaceSubsystem = GameInstance->GetSubsystem<USFRaceSubsystem>())
		{
			RaceSubsystem->SetRaceState(NewState);
			RaceSubsystem->SetActiveRaceId(ActiveRaceId);
			RaceSubsystem->SetSelectedRouteProfile(SelectedRouteProfile);
		}
	}
}

void ASFRaceManager::SetRaceState(ESFRaceState NewState)
{
	if (RaceState == NewState)
	{
		return;
	}

	RaceState = NewState;
	SyncSubsystemState(NewState);
	UE_LOG(LogSFRace, Log, TEXT("Race state -> %d"), static_cast<int32>(NewState));
	OnRaceStateChanged.Broadcast(NewState);

	if (NewState == ESFRaceState::Countdown || NewState == ESFRaceState::VehicleSpawn
		|| NewState == ESFRaceState::Results || NewState == ESFRaceState::DestinationReached)
	{
		SetVehicleDrivingEnabled(false);
	}
	else if (NewState == ESFRaceState::Racing)
	{
		SetVehicleDrivingEnabled(true);
	}
}

void ASFRaceManager::SetVehicleDrivingEnabled(bool bEnabled) const
{
	if (ASFVehiclePawn* Vehicle = FindPlayerVehicle())
	{
		Vehicle->SetDrivingEnabled(bEnabled);
	}
}

void ASFRaceManager::ResetPlayerToSpawn() const
{
	if (ASFVehiclePawn* Vehicle = FindPlayerVehicle())
	{
		Vehicle->ResetVehicle();
		Vehicle->SetDrivingEnabled(false);
	}
}

bool ASFRaceManager::ApplySelectedRouteToNavigation()
{
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData)
	{
		return false;
	}

	FSFRaceDefinitionData Race;
	if (!MapData->FindRace(ActiveRaceId, Race))
	{
		return false;
	}

	SuggestedRouteDistanceMeters = 0.0f;
	const FSFSuggestedRouteData* Selected = nullptr;
	for (const FSFSuggestedRouteData& Route : Race.Routes)
	{
		if (Route.Profile.Equals(SelectedRouteProfile, ESearchCase::IgnoreCase))
		{
			Selected = &Route;
			SuggestedRouteDistanceMeters = Route.DistanceMeters;
			break;
		}
	}
	if (!Selected && Race.Routes.Num() > 0)
	{
		Selected = &Race.Routes[0];
		SelectedRouteProfile = Selected->Profile;
		SuggestedRouteDistanceMeters = Selected->DistanceMeters;
	}
	if (!Selected)
	{
		return false;
	}

	if (USFNavigationSubsystem* Navigation = GetWorld()->GetSubsystem<USFNavigationSubsystem>())
	{
		FSFRoutePath Path;
		Path.EdgeIds = Selected->EdgeIds;
		Path.NodeIds = Selected->NodeIds;
		Path.DistanceMeters = Selected->DistanceMeters;
		Path.TravelTimeSeconds = Selected->EstimatedTimeSeconds;
		Path.bValid = true;
		Navigation->SetActiveRoute(Path, Selected->Profile);

		FSFLandmarkData DestinationLandmark;
		if (MapData->FindLandmark(Race.DestinationLandmarkId, DestinationLandmark)
			&& DestinationLandmark.bHasSpawn)
		{
			Navigation->SetDestinationNodeId(DestinationLandmark.Spawn.NodeId);
		}
	}

	SyncSubsystemState(RaceState);
	return true;
}

bool ASFRaceManager::ApplyRouteProfile(const FString& Profile)
{
	if (RaceState != ESFRaceState::Countdown && RaceState != ESFRaceState::VehicleSpawn
		&& RaceState != ESFRaceState::RaceSelection)
	{
		return false;
	}

	SelectedRouteProfile = Profile;
	return ApplySelectedRouteToNavigation();
}

bool ASFRaceManager::StartRace(const FString& RaceId, const FString& InSelectedRouteProfile)
{
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Error, TEXT("Cannot start race: map data unavailable"));
		return false;
	}

	FSFRaceDefinitionData Race;
	if (!MapData->FindRace(RaceId, Race))
	{
		UE_LOG(LogSFRace, Error, TEXT("Cannot start race: unknown race id %s"), *RaceId);
		return false;
	}

	ActiveRaceId = RaceId;
	SelectedRouteProfile = InSelectedRouteProfile;
	ElapsedRaceSeconds = 0.0f;
	DistanceDrivenMeters = 0.0f;
	RerouteCount = 0;
	CountdownValue = 4;
	CountdownTimer = 0.0f;
	bHasLastVehicleLocation = false;

	if (!ApplySelectedRouteToNavigation())
	{
		UE_LOG(LogSFRace, Warning, TEXT("StartRace: no route profile applied for %s"), *RaceId);
	}

	ResetPlayerToSpawn();

	if (ASFRouteRacerPlayerController* SFPC =
			Cast<ASFRouteRacerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		SFPC->HideResultsUI();
		SFPC->ShowRouteChoiceForRace(ActiveRaceId);
	}

	SetRaceState(ESFRaceState::VehicleSpawn);
	SetRaceState(ESFRaceState::Countdown);
	OnCountdownStep.Broadcast(CountdownValue);
	PushHud();
	return true;
}

void ASFRaceManager::RestartRace()
{
	if (ActiveRaceId.IsEmpty())
	{
		UE_LOG(LogSFRace, Warning, TEXT("Restart ignored: no active race"));
		return;
	}

	FString Profile = SelectedRouteProfile;
	if (ASFRouteRacerPlayerController* SFPC =
			Cast<ASFRouteRacerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		Profile = SFPC->GetSelectedRouteProfileFromUI();
		SFPC->HideResultsUI();
	}
	StartRace(ActiveRaceId, Profile);
}

void ASFRaceManager::ReturnToRaceSelection()
{
	SetRaceState(ESFRaceState::RaceSelection);
	if (ASFRouteRacerPlayerController* SFPC =
			Cast<ASFRouteRacerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		SFPC->HideResultsUI();
		SFPC->ShowRouteChoiceForRace(ActiveRaceId);
	}
}

void ASFRaceManager::NotifyReroute()
{
	if (RaceState == ESFRaceState::Racing)
	{
		++RerouteCount;
	}
}

void ASFRaceManager::BindDestinationMarker(ASFDestinationMarker* Marker)
{
	if (BoundDestination && BoundDestination->GetFinishSphere())
	{
		BoundDestination->GetFinishSphere()->OnComponentBeginOverlap.RemoveDynamic(
			this, &ASFRaceManager::HandleDestinationOverlap);
	}

	BoundDestination = Marker;
	if (BoundDestination)
	{
		BoundDestination->SetDestinationRadiusMeters(DestinationRadiusMeters);
		if (USphereComponent* Sphere = BoundDestination->GetFinishSphere())
		{
			Sphere->OnComponentBeginOverlap.AddDynamic(this, &ASFRaceManager::HandleDestinationOverlap);
		}
	}
}

void ASFRaceManager::AdvanceCountdown(float DeltaSeconds)
{
	CountdownTimer += DeltaSeconds;
	if (CountdownTimer < CountdownIntervalSeconds)
	{
		return;
	}

	CountdownTimer = 0.0f;
	--CountdownValue;

	if (USFAudioSubsystem* Audio = GetGameInstance()->GetSubsystem<USFAudioSubsystem>())
	{
		if (CountdownValue > 0)
		{
			Audio->PlayCountdownBeep();
		}
		else
		{
			Audio->PlayGoCue();
		}
	}

	if (CountdownValue > 0)
	{
		OnCountdownStep.Broadcast(CountdownValue);
		return;
	}

	OnCountdownStep.Broadcast(0); // GO
	if (ASFRouteRacerPlayerController* SFPC =
			Cast<ASFRouteRacerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		SelectedRouteProfile = SFPC->GetSelectedRouteProfileFromUI();
		ApplySelectedRouteToNavigation();
		SFPC->HideRouteChoice();
	}

	SetRaceState(ESFRaceState::Racing);
	if (ASFVehiclePawn* Vehicle = FindPlayerVehicle())
	{
		LastVehicleLocation = Vehicle->GetActorLocation();
		bHasLastVehicleLocation = true;
	}
}

void ASFRaceManager::UpdateRacing(float DeltaSeconds)
{
	ElapsedRaceSeconds += DeltaSeconds;
	if (ASFVehiclePawn* Vehicle = FindPlayerVehicle())
	{
		const FVector Location = Vehicle->GetActorLocation();
		if (bHasLastVehicleLocation)
		{
			DistanceDrivenMeters += FVector::Dist(LastVehicleLocation, Location) * 0.01f;
		}
		LastVehicleLocation = Location;
		bHasLastVehicleLocation = true;
	}
}

void ASFRaceManager::PushHud() const
{
	ASFRouteRacerPlayerController* SFPC =
		Cast<ASFRouteRacerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!SFPC)
	{
		return;
	}

	float Speed = 0.0f;
	if (const ASFVehiclePawn* Vehicle = FindPlayerVehicle())
	{
		Speed = Vehicle->GetSpeedKmh();
	}

	float DestMeters = 0.0f;
	bool bRerouting = false;
	FString Profile = SelectedRouteProfile;
	if (const USFNavigationSubsystem* Navigation = GetWorld()->GetSubsystem<USFNavigationSubsystem>())
	{
		DestMeters = Navigation->GetDestinationDistanceMeters();
		bRerouting = Navigation->IsRerouting();
		if (!Navigation->GetActiveProfile().IsEmpty())
		{
			Profile = Navigation->GetActiveProfile();
		}
	}

	UTextureRenderTarget2D* MinimapRT = nullptr;
	if (ASFMinimapCaptureActor* MinimapActor = Cast<ASFMinimapCaptureActor>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ASFMinimapCaptureActor::StaticClass())))
	{
		MinimapRT = MinimapActor->GetRenderTarget();
	}

	SFPC->PushRaceHudUpdate(
		RaceState,
		ElapsedRaceSeconds,
		Speed,
		DestMeters,
		DistanceDrivenMeters,
		SuggestedRouteDistanceMeters,
		Profile,
		bRerouting,
		CountdownValue,
		MinimapRT,
		120.0f);
}

void ASFRaceManager::HandleDestinationOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (RaceState != ESFRaceState::Racing)
	{
		return;
	}

	if (Cast<ASFVehiclePawn>(OtherActor) == nullptr && !OtherActor->IsA(ASFVehiclePawn::StaticClass()))
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (!Pawn || !Cast<APlayerController>(Pawn->GetController()))
		{
			return;
		}
	}

	CompleteRace();
}

void ASFRaceManager::CompleteRace()
{
	SetRaceState(ESFRaceState::DestinationReached);
	if (USFAudioSubsystem* Audio = GetGameInstance()->GetSubsystem<USFAudioSubsystem>())
	{
		Audio->PlayDestinationReached();
	}
	PersistBestTime();
	OnRaceFinished.Broadcast(ElapsedRaceSeconds);

	float BestTime = -1.0f;
	if (USFRaceSubsystem* RaceSubsystem = GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		BestTime = RaceSubsystem->GetBestTimeSeconds(ActiveRaceId);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ASFRouteRacerPlayerController* SFPC = Cast<ASFRouteRacerPlayerController>(PC))
		{
			SFPC->ShowResultsUI(
				ElapsedRaceSeconds,
				DistanceDrivenMeters,
				SuggestedRouteDistanceMeters,
				SelectedRouteProfile,
				RerouteCount,
				BestTime);
		}
	}

	SetRaceState(ESFRaceState::Results);
	UE_LOG(
		LogSFRace,
		Log,
		TEXT("Race finished: %.2fs, %.1fm driven, %d reroutes, profile=%s"),
		ElapsedRaceSeconds,
		DistanceDrivenMeters,
		RerouteCount,
		*SelectedRouteProfile);
}

void ASFRaceManager::PersistBestTime() const
{
	USFSaveGame* SaveObject = Cast<USFSaveGame>(UGameplayStatics::LoadGameFromSlot(USFSaveGame::SlotName, 0));
	if (!SaveObject)
	{
		SaveObject = Cast<USFSaveGame>(UGameplayStatics::CreateSaveGameObject(USFSaveGame::StaticClass()));
	}
	if (!SaveObject)
	{
		return;
	}

	SaveObject->LastPlayedRaceId = ActiveRaceId;
	if (const float* Existing = SaveObject->BestTimesSeconds.Find(ActiveRaceId))
	{
		if (ElapsedRaceSeconds < *Existing)
		{
			SaveObject->BestTimesSeconds.Add(ActiveRaceId, ElapsedRaceSeconds);
		}
	}
	else
	{
		SaveObject->BestTimesSeconds.Add(ActiveRaceId, ElapsedRaceSeconds);
	}

	UGameplayStatics::SaveGameToSlot(SaveObject, USFSaveGame::SlotName, 0);
}

ASFVehiclePawn* ASFRaceManager::FindPlayerVehicle() const
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		return Cast<ASFVehiclePawn>(PC->GetPawn());
	}
	return nullptr;
}
