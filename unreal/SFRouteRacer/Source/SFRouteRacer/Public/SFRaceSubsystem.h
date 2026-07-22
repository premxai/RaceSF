#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SFRaceManager.h"
#include "SFRaceSubsystem.generated.h"

/**
 * Shared race session state for later menu/HUD wiring.
 */
UCLASS()
class SFROUTERACER_API USFRaceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetActiveRaceId(const FString& RaceId);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	FString GetActiveRaceId() const { return ActiveRaceId; }

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetRaceState(ESFRaceState NewState);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ESFRaceState GetRaceState() const { return RaceState; }

private:
	UPROPERTY()
	FString ActiveRaceId;

	UPROPERTY()
	ESFRaceState RaceState = ESFRaceState::Idle;
};
