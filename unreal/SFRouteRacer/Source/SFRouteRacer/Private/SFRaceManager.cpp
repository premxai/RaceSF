#include "SFRaceManager.h"

#include "SFRouteRacer.h"

ASFRaceManager::ASFRaceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASFRaceManager::SetRaceState(ESFRaceState NewState)
{
	RaceState = NewState;
	UE_LOG(LogSFRace, Log, TEXT("Race state -> %d"), static_cast<int32>(NewState));
}
