#include "SFGeoRuntime.h"

#define LOCTEXT_NAMESPACE "FSFGeoRuntimeModule"

DEFINE_LOG_CATEGORY(LogSFGeo);
DEFINE_LOG_CATEGORY(LogSFRouting);
DEFINE_LOG_CATEGORY(LogSFMapGeneration);

void FSFGeoRuntimeModule::StartupModule()
{
	UE_LOG(LogSFGeo, Log, TEXT("SFGeoRuntime started"));
}

void FSFGeoRuntimeModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSFGeoRuntimeModule, SFGeoRuntime)
