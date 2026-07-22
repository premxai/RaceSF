#include "SFCreditsWidget.h"

#include "Engine/GameInstance.h"
#include "SFMapDataSubsystem.h"

TArray<FString> USFCreditsWidget::GetAttributionLines() const
{
	TArray<FString> Lines;
	Lines.Add(TEXT("SF Route Racer"));
	Lines.Add(TEXT("Open-city destination racing prototype"));
	Lines.Add(TEXT(""));

	if (UGameInstance* GameInstance = GetOwningPlayer() ? GetOwningPlayer()->GetGameInstance() : nullptr)
	{
		if (USFMapDataSubsystem* MapData = GameInstance->GetSubsystem<USFMapDataSubsystem>())
		{
			if (MapData->IsMapLoaded())
			{
				Lines.Append(MapData->GetManifest().Attribution);
				for (const TPair<FString, FString>& Pair : MapData->GetManifest().SourceDatasetVersions)
				{
					Lines.Add(FString::Printf(TEXT("%s: %s"), *Pair.Key, *Pair.Value));
				}
			}
		}
	}

	if (Lines.Num() <= 3)
	{
		Lines.Add(TEXT("© OpenStreetMap contributors, ODbL 1.0"));
		Lines.Add(TEXT("Overture Maps Foundation data"));
	}

	Lines.Add(TEXT(""));
	Lines.Add(TEXT("No Google Maps, Street View, or Google 3D Tiles are used."));
	return Lines;
}
