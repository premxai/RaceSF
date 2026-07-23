#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SFRaceManager.h"
#include "SFRouteRacerPlayerController.generated.h"

class USFVehicleHUDWidget;
class USFResultsWidget;
class USFRouteChoiceWidget;

/**
 * Player controller for vehicle possession, HUD, route choice, and results.
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

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void HideResultsUI();

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void ShowRouteChoiceForRace(const FString& RaceId);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void HideRouteChoice();

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void PushRaceHudUpdate(
		ESFRaceState RaceState,
		float ElapsedSeconds,
		float SpeedKmh,
		float DestinationMeters,
		float DrivenMeters,
		float SuggestedMeters,
		const FString& RouteProfile,
		bool bRerouting,
		int32 CountdownValue,
		class UTextureRenderTarget2D* MinimapTexture,
		float MaxSpeedKmh);

	UFUNCTION(BlueprintPure, Category = "SF|UI")
	USFVehicleHUDWidget* GetHUDWidget() const { return HUDWidget; }

	UFUNCTION(BlueprintPure, Category = "SF|UI")
	FString GetSelectedRouteProfileFromUI() const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void HandleRestartRequested();

	UFUNCTION()
	void HandleReturnRequested();

	UFUNCTION()
	void HandleRouteProfileSelected(const FString& Profile);

	void EnsureWidgets();
	void RestoreGameplayInput();

	UPROPERTY(EditAnywhere, Category = "SF|UI")
	TSubclassOf<USFVehicleHUDWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "SF|UI")
	TSubclassOf<USFResultsWidget> ResultsWidgetClass;

	UPROPERTY(EditAnywhere, Category = "SF|UI")
	TSubclassOf<USFRouteChoiceWidget> RouteChoiceWidgetClass;

	UPROPERTY()
	TObjectPtr<USFVehicleHUDWidget> HUDWidget;

	UPROPERTY()
	TObjectPtr<USFResultsWidget> ResultsWidget;

	UPROPERTY()
	TObjectPtr<USFRouteChoiceWidget> RouteChoiceWidget;
};
