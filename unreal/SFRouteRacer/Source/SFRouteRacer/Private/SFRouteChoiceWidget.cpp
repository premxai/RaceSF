#include "SFRouteChoiceWidget.h"

#include "Engine/GameInstance.h"
#include "SFMapDataSubsystem.h"

void USFRouteChoiceWidget::BuildCardsForRace(const FString& RaceId, TArray<FSFRouteCardView>& OutCards)
{
	OutCards.Reset();
	UGameInstance* GameInstance = GetGameInstance();
	USFMapDataSubsystem* MapData = GameInstance ? GameInstance->GetSubsystem<USFMapDataSubsystem>() : nullptr;
	if (!MapData)
	{
		return;
	}

	FSFRaceDefinitionData Race;
	if (!MapData->FindRace(RaceId, Race) || Race.Routes.Num() == 0)
	{
		return;
	}

	const float FastestTime = Race.Routes[0].EstimatedTimeSeconds;
	for (const FSFSuggestedRouteData& Route : Race.Routes)
	{
		FSFRouteCardView Card;
		Card.Profile = Route.Profile;
		Card.DistanceMeters = Route.DistanceMeters;
		Card.EstimatedTimeSeconds = Route.EstimatedTimeSeconds;
		Card.DeltaSecondsFromFastest = Route.EstimatedTimeSeconds - FastestTime;
		OutCards.Add(Card);
	}
}
