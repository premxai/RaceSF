#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SFSaveGame.generated.h"

/**
 * Local-only save data for best times and player preferences.
 */
UCLASS()
class SFROUTERACER_API USFSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static constexpr const TCHAR* SlotName = TEXT("SFRouteRacerSave");

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	TMap<FString, float> BestTimesSeconds;

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	FString LastPlayedRaceId;

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	FString PreferredUnits = TEXT("metric");

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	FString MinimapMode = TEXT("rotate");

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	float MasterVolume = 1.0f;

	UPROPERTY(VisibleAnywhere, Category = "SF|Save")
	float InputSensitivity = 1.0f;
};
