#include "SFRaceSubsystem.h"

#include "SFRouteRacer.h"

void USFRaceSubsystem::SetActiveRaceId(const FString& RaceId)
{
	ActiveRaceId = RaceId;
}

void USFRaceSubsystem::SetRaceState(ESFRaceState NewState)
{
	RaceState = NewState;
	UE_LOG(LogSFRace, Log, TEXT("Race subsystem state -> %d"), static_cast<int32>(NewState));
}
