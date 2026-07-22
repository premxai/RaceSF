#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFRaceManager.generated.h"

UENUM(BlueprintType)
enum class ESFRaceState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Loading UMETA(DisplayName = "Loading"),
	Countdown UMETA(DisplayName = "Countdown"),
	Racing UMETA(DisplayName = "Racing"),
	Finished UMETA(DisplayName = "Finished")
};

/**
 * Minimal race state holder for Milestone 2. Full loop arrives in Milestone 3.
 */
UCLASS()
class SFROUTERACER_API ASFRaceManager : public AActor
{
	GENERATED_BODY()

public:
	ASFRaceManager();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetRaceState(ESFRaceState NewState);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ESFRaceState GetRaceState() const { return RaceState; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	ESFRaceState RaceState = ESFRaceState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString ActiveRaceId;
};
