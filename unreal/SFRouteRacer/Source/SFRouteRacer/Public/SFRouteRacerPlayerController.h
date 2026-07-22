#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SFRouteRacerPlayerController.generated.h"

class USFVehicleHUDWidget;
class USFResultsWidget;

/**
 * Player controller for vehicle possession, HUD, and results presentation.
 */
UCLASS()
class SFROUTERACER_API ASFRouteRacerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASFRouteRacerPlayerController();

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void ShowResultsUI(
		float CompletionSeconds,
		float DistanceMeters,
		float SuggestedDistanceMeters,
		const FString& SelectedRouteProfile,
		int32 RerouteCount,
		float BestTimeSeconds);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "SF|UI")
	TSubclassOf<USFVehicleHUDWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "SF|UI")
	TSubclassOf<USFResultsWidget> ResultsWidgetClass;

	UPROPERTY()
	TObjectPtr<USFVehicleHUDWidget> HUDWidget;

	UPROPERTY()
	TObjectPtr<USFResultsWidget> ResultsWidget;
};
