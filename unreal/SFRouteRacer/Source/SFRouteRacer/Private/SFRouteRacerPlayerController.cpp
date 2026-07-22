#include "SFRouteRacerPlayerController.h"

#include "SFResultsWidget.h"
#include "SFRouteRacer.h"
#include "SFVehicleHUDWidget.h"

ASFRouteRacerPlayerController::ASFRouteRacerPlayerController()
{
	bShowMouseCursor = false;
}

void ASFRouteRacerPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USFVehicleHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}

void ASFRouteRacerPlayerController::ShowResultsUI(
	float CompletionSeconds,
	float DistanceMeters,
	float SuggestedDistanceMeters,
	const FString& SelectedRouteProfile,
	int32 RerouteCount,
	float BestTimeSeconds)
{
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	if (!ResultsWidget && ResultsWidgetClass)
	{
		ResultsWidget = CreateWidget<USFResultsWidget>(this, ResultsWidgetClass);
	}
	if (ResultsWidget)
	{
		ResultsWidget->AddToViewport(10);
		ResultsWidget->ShowResults(
			CompletionSeconds,
			DistanceMeters,
			SuggestedDistanceMeters,
			SelectedRouteProfile,
			RerouteCount,
			BestTimeSeconds);
	}
	else
	{
		UE_LOG(
			LogSFRace,
			Log,
			TEXT("Results (no widget class): %.2fs / %.1fm / profile=%s / reroutes=%d / best=%.2f"),
			CompletionSeconds,
			DistanceMeters,
			*SelectedRouteProfile,
			RerouteCount,
			BestTimeSeconds);
	}
}
