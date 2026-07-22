#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFRaceSelectionWidget.generated.h"

UCLASS()
class SFROUTERACER_API USFRaceSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void PopulateRaces();
};
