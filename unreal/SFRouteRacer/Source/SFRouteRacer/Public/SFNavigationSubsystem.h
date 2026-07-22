#pragma once

#include "CoreMinimal.h"
#include "SFRoutingSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFNavigationSubsystem.generated.h"

class ASFRouteHighlightActor;
class ASFDestinationMarker;
class ASFMinimapCaptureActor;

/**
 * Runtime navigation: active route, destination distance, and throttled rerouting.
 */
UCLASS()
class SFROUTERACER_API USFNavigationSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableInEditor() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void SetActiveRoute(const FSFRoutePath& Route, const FString& Profile);

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void SetDestinationNodeId(const FString& NodeId);

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void BindActors(
		ASFRouteHighlightActor* Highlight,
		ASFDestinationMarker* Destination,
		ASFMinimapCaptureActor* Minimap);

	UFUNCTION(BlueprintPure, Category = "SF|Navigation")
	float GetDestinationDistanceMeters() const { return DestinationDistanceMeters; }

	UFUNCTION(BlueprintPure, Category = "SF|Navigation")
	bool IsRerouting() const { return bReroutingVisible; }

	UFUNCTION(BlueprintPure, Category = "SF|Navigation")
	FString GetActiveProfile() const { return ActiveProfile; }

	UFUNCTION(BlueprintPure, Category = "SF|Navigation")
	const FSFRoutePath& GetActiveRoute() const { return ActiveRoute; }

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	bool ForceRerouteFromVehicle();

protected:
	bool ShouldReroute(const FVector& VehicleLocalMeters) const;
	void ApplyRoute(const FSFRoutePath& Route, const FString& Profile, bool bCountAsReroute);

	UPROPERTY()
	FSFRoutePath ActiveRoute;

	UPROPERTY()
	FString ActiveProfile = TEXT("fastest");

	UPROPERTY()
	FString DestinationNodeId;

	UPROPERTY()
	TWeakObjectPtr<ASFRouteHighlightActor> HighlightActor;

	UPROPERTY()
	TWeakObjectPtr<ASFDestinationMarker> DestinationMarker;

	UPROPERTY()
	TWeakObjectPtr<ASFMinimapCaptureActor> MinimapActor;

	float DestinationDistanceMeters = 0.0f;
	float RerouteCooldownSeconds = 0.5f;
	float TimeSinceLastReroute = 0.0f;
	float RerouteBannerSeconds = 0.0f;
	bool bReroutingVisible = false;
	float OffRouteThresholdMeters = 35.0f;
};
