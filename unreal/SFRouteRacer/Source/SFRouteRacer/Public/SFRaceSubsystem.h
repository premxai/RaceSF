#pragma once

#include "CoreMinimal.h"
#include "SFRaceManager.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SFRaceSubsystem.generated.h"

/**
 * Shared race session helpers: choose race, quick race, daily run prototype.
 */
UCLASS()
class SFROUTERACER_API USFRaceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetActiveRaceId(const FString& RaceId);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	FString GetActiveRaceId() const { return ActiveRaceId; }

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetSelectedRouteProfile(const FString& Profile);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	FString GetSelectedRouteProfile() const { return SelectedRouteProfile; }

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetRaceState(ESFRaceState NewState);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ESFRaceState GetRaceState() const { return RaceState; }

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool ChooseRace(const FString& StartLandmarkId, const FString& DestinationLandmarkId, FString& OutRaceId);

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool SelectQuickRace(FString& OutRaceId);

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool SelectDailyRun(FString& OutRaceId);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	float GetBestTimeSeconds(const FString& RaceId) const;

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void RegisterRaceManager(ASFRaceManager* Manager);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ASFRaceManager* GetRaceManager() const { return RaceManager.Get(); }

private:
	bool IsValidQuickRacePair(const struct FSFRaceDefinitionData& Race) const;

	UPROPERTY()
	FString ActiveRaceId;

	UPROPERTY()
	FString SelectedRouteProfile = TEXT("fastest");

	UPROPERTY()
	FString PreviousQuickRaceId;

	UPROPERTY()
	ESFRaceState RaceState = ESFRaceState::MainMenu;

	UPROPERTY()
	TWeakObjectPtr<ASFRaceManager> RaceManager;
};
