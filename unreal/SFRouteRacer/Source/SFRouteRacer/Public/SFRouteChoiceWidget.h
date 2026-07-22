#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFRouteChoiceWidget.generated.h"

UCLASS()
class SFROUTERACER_API USFRouteChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void ShowRoutesForRace(const FString& RaceId);
};
