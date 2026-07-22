#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSFGeo, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSFRouting, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogSFMapGeneration, Log, All);

class FSFGeoRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
