#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SFRouteRacerGameMode.generated.h"

class ASFRoadNetworkActor;
class ASFBuildingTileActor;
class ASFDestinationMarker;
class ASFVehiclePawn;

/**
 * Graybox map bootstrap and default pawn selection for the SF MVP.
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

protected:
	UPROPERTY(EditAnywhere, Category = "SF|Map")
	TSubclassOf<ASFVehiclePawn> VehicleClass;

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	FString DefaultRaceId = TEXT("ferry_building_to_chase_center");

	UPROPERTY(EditAnywhere, Category = "SF|Map")
	bool bSpawnMapActorsOnStart = true;

	UPROPERTY()
	TObjectPtr<ASFRoadNetworkActor> RoadNetworkActor;

	UPROPERTY()
	TObjectPtr<ASFBuildingTileActor> BuildingTileActor;

	UPROPERTY()
	TObjectPtr<ASFDestinationMarker> DestinationMarker;
};
