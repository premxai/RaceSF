#include "SFRouteRacerPlayerController.h"

#include "Engine/GameInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "SFNavigationSubsystem.h"
#include "SFRaceManager.h"
#include "SFRaceSubsystem.h"
#include "SFResultsWidget.h"
#include "SFRouteChoiceWidget.h"
#include "SFRouteRacer.h"
#include "SFRouteRacerGameMode.h"
#include "SFVehicleHUDWidget.h"
#include "SFVehiclePawn.h"

ASFRouteRacerPlayerController::ASFRouteRacerPlayerController()
{
	bShowMouseCursor = false;
	PrimaryActorTick.bCanEverTick = true;
	HUDWidgetClass = USFVehicleHUDWidget::StaticClass();
	ResultsWidgetClass = USFResultsWidget::StaticClass();
	RouteChoiceWidgetClass = USFRouteChoiceWidget::StaticClass();
}

void ASFRouteRacerPlayerController::BeginPlay()
{
	Super::BeginPlay();
	EnsureWidgets();
}

void ASFRouteRacerPlayerController::EnsureWidgets()
{
	if (!HUDWidget && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<USFVehicleHUDWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport(0);
			HUDWidget->SetDestinationLabel(TEXT("Chase Center"));
		}
	}

	if (!ResultsWidget && ResultsWidgetClass)
	{
		ResultsWidget = CreateWidget<USFResultsWidget>(this, ResultsWidgetClass);
		if (ResultsWidget)
		{
			ResultsWidget->AddToViewport(20);
			ResultsWidget->HideResults();
			ResultsWidget->OnRestartRequested.AddDynamic(this, &ASFRouteRacerPlayerController::HandleRestartRequested);
			ResultsWidget->OnReturnRequested.AddDynamic(this, &ASFRouteRacerPlayerController::HandleReturnRequested);
		}
	}

	if (!RouteChoiceWidget && RouteChoiceWidgetClass)
	{
		RouteChoiceWidget = CreateWidget<USFRouteChoiceWidget>(this, RouteChoiceWidgetClass);
		if (RouteChoiceWidget)
		{
			RouteChoiceWidget->AddToViewport(5);
			RouteChoiceWidget->HideCards();
			RouteChoiceWidget->OnRouteProfileSelected.AddDynamic(
				this, &ASFRouteRacerPlayerController::HandleRouteProfileSelected);
		}
	}
}

void ASFRouteRacerPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	(void)DeltaSeconds;
}

void ASFRouteRacerPlayerController::PushRaceHudUpdate(
	ESFRaceState RaceState,
	float ElapsedSeconds,
	float SpeedKmh,
	float DestinationMeters,
	float DrivenMeters,
	float SuggestedMeters,
	const FString& RouteProfile,
	bool bRerouting,
	int32 CountdownValue,
	UTextureRenderTarget2D* MinimapTexture,
	float MaxSpeedKmh)
{
	EnsureWidgets();
	if (!HUDWidget)
	{
		return;
	}

	HUDWidget->SetRaceStateDisplay(RaceState);
	HUDWidget->UpdateRaceTimer(ElapsedSeconds);
	HUDWidget->UpdateSpeed(SpeedKmh);
	HUDWidget->UpdateDestinationDistance(DestinationMeters);
	HUDWidget->UpdateDrivenDistance(DrivenMeters);
	HUDWidget->UpdateSuggestedDistance(SuggestedMeters);
	HUDWidget->SetActiveRouteProfile(RouteProfile);
	HUDWidget->SetReroutingVisible(bRerouting);
	HUDWidget->SetMaxSpeedKmh(MaxSpeedKmh);
	if (MinimapTexture)
	{
		HUDWidget->SetMinimapTexture(MinimapTexture);
	}
	if (RaceState == ESFRaceState::Countdown)
	{
		HUDWidget->SetCountdownDisplay(CountdownValue);
	}
}

void ASFRouteRacerPlayerController::ShowRouteChoiceForRace(const FString& RaceId)
{
	EnsureWidgets();
	if (!RouteChoiceWidget)
	{
		return;
	}

	TArray<FSFRouteCardView> Cards;
	RouteChoiceWidget->BuildCardsForRace(RaceId, Cards);
	RouteChoiceWidget->ShowRouteCards(Cards);
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameAndUI()
					 .SetHideCursorDuringCapture(true)
					 .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
					 .SetWidgetToFocus(RouteChoiceWidget->TakeWidget()));
}

void ASFRouteRacerPlayerController::HideRouteChoice()
{
	if (RouteChoiceWidget)
	{
		RouteChoiceWidget->HideCards();
	}
	RestoreGameplayInput();
}

FString ASFRouteRacerPlayerController::GetSelectedRouteProfileFromUI() const
{
	if (RouteChoiceWidget)
	{
		return RouteChoiceWidget->GetSelectedProfile();
	}
	return TEXT("fastest");
}

void ASFRouteRacerPlayerController::ShowResultsUI(
	float CompletionSeconds,
	float DistanceMeters,
	float SuggestedDistanceMeters,
	const FString& SelectedRouteProfile,
	int32 RerouteCount,
	float BestTimeSeconds)
{
	EnsureWidgets();
	if (RouteChoiceWidget)
	{
		RouteChoiceWidget->HideCards();
	}
	bShowMouseCursor = false;

	if (ResultsWidget)
	{
		SetInputMode(FInputModeGameAndUI()
						 .SetHideCursorDuringCapture(true)
						 .SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock)
						 .SetWidgetToFocus(ResultsWidget->TakeWidget()));
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
			TEXT("Results (no widget): %.2fs / %.1fm / profile=%s / reroutes=%d / best=%.2f"),
			CompletionSeconds,
			DistanceMeters,
			*SelectedRouteProfile,
			RerouteCount,
			BestTimeSeconds);
	}
}

void ASFRouteRacerPlayerController::HideResultsUI()
{
	if (ResultsWidget)
	{
		ResultsWidget->HideResults();
	}
	RestoreGameplayInput();
}

void ASFRouteRacerPlayerController::RestoreGameplayInput()
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

void ASFRouteRacerPlayerController::HandleRouteProfileSelected(const FString& Profile)
{
	if (USFRaceSubsystem* RaceSubsystem = GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		RaceSubsystem->SetSelectedRouteProfile(Profile);
		if (ASFRaceManager* Manager = RaceSubsystem->GetRaceManager())
		{
			if (Manager->GetRaceState() == ESFRaceState::Countdown)
			{
				Manager->ApplyRouteProfile(Profile);
			}
		}
	}
}

void ASFRouteRacerPlayerController::HandleRestartRequested()
{
	HideResultsUI();
	if (USFRaceSubsystem* RaceSubsystem = GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		if (ASFRaceManager* Manager = RaceSubsystem->GetRaceManager())
		{
			Manager->RestartRace();
			return;
		}
	}

	if (ASFRouteRacerGameMode* GameMode = GetWorld()->GetAuthGameMode<ASFRouteRacerGameMode>())
	{
		GameMode->BeginDefaultRace();
	}
}

void ASFRouteRacerPlayerController::HandleReturnRequested()
{
	HideResultsUI();
	if (USFRaceSubsystem* RaceSubsystem = GetGameInstance()->GetSubsystem<USFRaceSubsystem>())
	{
		if (ASFRaceManager* Manager = RaceSubsystem->GetRaceManager())
		{
			Manager->ReturnToRaceSelection();
			Manager->RestartRace();
		}
	}
}
