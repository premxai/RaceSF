#include "SFRouteRacer.h"

DEFINE_LOG_CATEGORY(LogSFRace);

void FSFRouteRacerModule::StartupModule()
{
	UE_LOG(LogSFRace, Log, TEXT("SFRouteRacer gameplay module started"));
}

void FSFRouteRacerModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FSFRouteRacerModule, SFRouteRacer, "SFRouteRacer")
