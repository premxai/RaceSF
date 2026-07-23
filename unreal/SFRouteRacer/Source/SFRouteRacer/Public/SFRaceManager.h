#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFRaceManager.generated.h"

class ASFDestinationMarker;
class ASFVehiclePawn;

UENUM(BlueprintType)
enum class ESFRaceState : uint8
{
	MainMenu UMETA(DisplayName = "Main Menu"),
	RaceSelection UMETA(DisplayName = "Race Selection"),
	MapLoading UMETA(DisplayName = "Map Loading"),
	VehicleSpawn UMETA(DisplayName = "Vehicle Spawn"),
	Countdown UMETA(DisplayName = "Countdown"),
	Racing UMETA(DisplayName = "Racing"),
	DestinationReached UMETA(DisplayName = "Destination Reached"),
	Results UMETA(DisplayName = "Results")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSFRaceStateChanged, ESFRaceState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSFCountdownStep, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSFRaceFinished, float, CompletionSeconds);

/**
 * Owns the Milestone 3 race loop: countdown, timer, finish, and results.
 */
UCLASS()
class SFROUTERACER_API ASFRaceManager : public AActor
{
	GENERATED_BODY()

public:
	ASFRaceManager();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool StartRace(const FString& RaceId, const FString& SelectedRouteProfile = TEXT("fastest"));

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool ApplyRouteProfile(const FString& Profile);

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void RestartRace();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void ReturnToRaceSelection();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void SetRaceState(ESFRaceState NewState);

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ESFRaceState GetRaceState() const { return RaceState; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	float GetElapsedRaceSeconds() const { return ElapsedRaceSeconds; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	float GetDistanceDrivenMeters() const { return DistanceDrivenMeters; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	int32 GetRerouteCount() const { return RerouteCount; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	int32 GetCountdownValue() const { return CountdownValue; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	FString GetActiveRaceId() const { return ActiveRaceId; }

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	FString GetSelectedRouteProfile() const { return SelectedRouteProfile; }

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void NotifyReroute();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	void BindDestinationMarker(ASFDestinationMarker* Marker);

	UPROPERTY(BlueprintAssignable, Category = "SF|Race")
	FSFRaceStateChanged OnRaceStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "SF|Race")
	FSFCountdownStep OnCountdownStep;

	UPROPERTY(BlueprintAssignable, Category = "SF|Race")
	FSFRaceFinished OnRaceFinished;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDestinationOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void AdvanceCountdown(float DeltaSeconds);
	void UpdateRacing(float DeltaSeconds);
	void CompleteRace();
	void PersistBestTime() const;
	void SyncSubsystemState(ESFRaceState NewState) const;
	void SetVehicleDrivingEnabled(bool bEnabled) const;
	void ResetPlayerToSpawn() const;
	void PushHud() const;
	bool ApplySelectedRouteToNavigation();
	ASFVehiclePawn* FindPlayerVehicle() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	ESFRaceState RaceState = ESFRaceState::MainMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString ActiveRaceId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SF|Race")
	FString SelectedRouteProfile = TEXT("fastest");

	UPROPERTY(EditAnywhere, Category = "SF|Race")
	float DestinationRadiusMeters = 25.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Race")
	float CountdownIntervalSeconds = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	float ElapsedRaceSeconds = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	float DistanceDrivenMeters = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	float SuggestedRouteDistanceMeters = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Race")
	int32 RerouteCount = 0;

	UPROPERTY()
	TObjectPtr<ASFDestinationMarker> BoundDestination;

	int32 CountdownValue = 3;
	float CountdownTimer = 0.0f;
	FVector LastVehicleLocation = FVector::ZeroVector;
	bool bHasLastVehicleLocation = false;
};
