#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SFRouteRacerGameMode.generated.h"

class ASFRoadNetworkActor;
class ASFBuildingTileActor;
class ASFDestinationMarker;
class ASFRaceManager;
class ASFRouteHighlightActor;
class ASFMinimapCaptureActor;
class ASFGhostOpponentActor;
class ASFBoundaryGeneratorActor;
class ASFDebugDrawActor;
class ASFVehiclePawn;

/**
 * Graybox map bootstrap, race manager spawn, and default pawn selection.
 */
UCLASS()
class SFROUTERACER_API ASFRouteRacerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASFRouteRacerGameMode();

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	bool BootstrapGrayboxWorld();

	UFUNCTION(BlueprintCallable, Category = "SF|Map")
	void EnsureWorldLighting();

	UFUNCTION(BlueprintCallable, Category = "SF|Race")
	bool BeginDefaultRace();

	UFUNCTION(BlueprintPure, Category = "SF|Race")
	ASFRaceManager* GetRaceManager() const { return RaceManager; }

protected:
	UPROPERTY(EditAnywhere, Category = "SF|Map")
	TSubclassOf<ASFVehiclePawn> VehicleClass;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	FString DefaultRaceId = TEXT("ferry_building_to_chase_center");

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	bool bSpawnMapActorsOnStart = true;

	UPROPERTY(EditAnywhere, Category = "SF|Race")
	bool bAutoStartDefaultRace = true;

	UPROPERTY(EditAnywhere, Category = "SF|AI")
	bool bSpawnGhostOpponent = true;

	UPROPERTY(EditAnywhere, Category = "SF|AI")
	FString GhostRouteProfile = TEXT("scenic");

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bEnableDebugDraw = false;

	UPROPERTY()
	TObjectPtr<ASFRoadNetworkActor> RoadNetworkActor;

	UPROPERTY()
	TObjectPtr<ASFBuildingTileActor> BuildingTileActor;

	UPROPERTY()
	TObjectPtr<ASFDestinationMarker> DestinationMarker;

	UPROPERTY()
	TObjectPtr<ASFDestinationMarker> StartMarker;

	UPROPERTY()
	TObjectPtr<ASFRaceManager> RaceManager;

	UPROPERTY()
	TObjectPtr<ASFRouteHighlightActor> RouteHighlightActor;

	UPROPERTY()
	TObjectPtr<ASFMinimapCaptureActor> MinimapCaptureActor;

	UPROPERTY()
	TObjectPtr<ASFGhostOpponentActor> GhostOpponent;

	UPROPERTY()
	TObjectPtr<ASFBoundaryGeneratorActor> BoundaryGenerator;

	UPROPERTY()
	TObjectPtr<ASFDebugDrawActor> DebugDrawActor;
};
