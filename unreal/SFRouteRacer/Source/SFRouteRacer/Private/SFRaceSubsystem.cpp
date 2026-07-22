#include "SFRaceSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/DateTime.h"
#include "SFMapDataSubsystem.h"
#include "SFRouteRacer.h"
#include "SFSaveGame.h"

void USFRaceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USFRaceSubsystem::SetActiveRaceId(const FString& RaceId)
{
	ActiveRaceId = RaceId;
}

void USFRaceSubsystem::SetSelectedRouteProfile(const FString& Profile)
{
	SelectedRouteProfile = Profile;
}

void USFRaceSubsystem::SetRaceState(ESFRaceState NewState)
{
	RaceState = NewState;
	UE_LOG(LogSFRace, Log, TEXT("Race subsystem state -> %d"), static_cast<int32>(NewState));
}

void USFRaceSubsystem::RegisterRaceManager(ASFRaceManager* Manager)
{
	RaceManager = Manager;
}

bool USFRaceSubsystem::IsValidQuickRacePair(const FSFRaceDefinitionData& Race) const
{
	if (Race.Routes.Num() < 3)
	{
		return false;
	}
	float MinDistance = TNumericLimits<float>::Max();
	for (const FSFSuggestedRouteData& Route : Race.Routes)
	{
		MinDistance = FMath::Min(MinDistance, Route.DistanceMeters);
	}
	return MinDistance >= 1500.0f && Race.StartLandmarkId != Race.DestinationLandmarkId;
}

bool USFRaceSubsystem::ChooseRace(
	const FString& StartLandmarkId,
	const FString& DestinationLandmarkId,
	FString& OutRaceId)
{
	if (StartLandmarkId == DestinationLandmarkId)
	{
		UE_LOG(LogSFRace, Warning, TEXT("ChooseRace rejected: start and destination are identical"));
		return false;
	}

	USFMapDataSubsystem* MapData = GetGameInstance()->GetSubsystem<USFMapDataSubsystem>();
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		return false;
	}

	for (const FSFRaceDefinitionData& Race : MapData->GetRaces())
	{
		if (Race.StartLandmarkId == StartLandmarkId && Race.DestinationLandmarkId == DestinationLandmarkId)
		{
			if (Race.Routes.Num() < 3)
			{
				UE_LOG(LogSFRace, Warning, TEXT("Race %s has fewer than three routes"), *Race.RaceId);
				return false;
			}
			OutRaceId = Race.RaceId;
			ActiveRaceId = OutRaceId;
			SetRaceState(ESFRaceState::RaceSelection);
			return true;
		}
	}

	UE_LOG(
		LogSFRace,
		Warning,
		TEXT("No validated race for %s -> %s"),
		*StartLandmarkId,
		*DestinationLandmarkId);
	return false;
}

bool USFRaceSubsystem::SelectQuickRace(FString& OutRaceId)
{
	USFMapDataSubsystem* MapData = GetGameInstance()->GetSubsystem<USFMapDataSubsystem>();
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		UE_LOG(LogSFRace, Error, TEXT("Quick Race failed: map data unavailable"));
		return false;
	}

	TArray<FString> Candidates;
	for (const FSFRaceDefinitionData& Race : MapData->GetRaces())
	{
		if (IsValidQuickRacePair(Race) && Race.RaceId != PreviousQuickRaceId)
		{
			Candidates.Add(Race.RaceId);
		}
	}

	if (Candidates.Num() == 0)
	{
		for (const FSFRaceDefinitionData& Race : MapData->GetRaces())
		{
			if (IsValidQuickRacePair(Race))
			{
				Candidates.Add(Race.RaceId);
			}
		}
	}

	if (Candidates.Num() == 0)
	{
		UE_LOG(LogSFRace, Error, TEXT("Quick Race failed: no valid pairs available"));
		return false;
	}

	const int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
	OutRaceId = Candidates[Index];
	PreviousQuickRaceId = OutRaceId;
	ActiveRaceId = OutRaceId;
	SetRaceState(ESFRaceState::RaceSelection);
	return true;
}

bool USFRaceSubsystem::SelectDailyRun(FString& OutRaceId)
{
	USFMapDataSubsystem* MapData = GetGameInstance()->GetSubsystem<USFMapDataSubsystem>();
	if (!MapData || (!MapData->IsMapLoaded() && !MapData->LoadDefaultExport()))
	{
		return false;
	}

	const TArray<FSFRaceDefinitionData>& Races = MapData->GetRaces();
	if (Races.Num() == 0)
	{
		UE_LOG(LogSFRace, Warning, TEXT("Daily Run prototype has no races"));
		return false;
	}

	const FDateTime Now = FDateTime::Now();
	const int32 DayKey = Now.GetYear() * 1000 + Now.GetDayOfYear();
	const int32 Index = DayKey % Races.Num();
	OutRaceId = Races[Index].RaceId;
	ActiveRaceId = OutRaceId;
	UE_LOG(LogSFRace, Log, TEXT("Daily Run (local prototype) selected %s"), *OutRaceId);
	return true;
}

float USFRaceSubsystem::GetBestTimeSeconds(const FString& RaceId) const
{
	if (const USFSaveGame* SaveObject = Cast<USFSaveGame>(UGameplayStatics::LoadGameFromSlot(USFSaveGame::SlotName, 0)))
	{
		if (const float* Best = SaveObject->BestTimesSeconds.Find(RaceId))
		{
			return *Best;
		}
	}
	return -1.0f;
}
