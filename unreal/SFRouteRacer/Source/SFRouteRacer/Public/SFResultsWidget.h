#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFResultsWidget.generated.h"

UCLASS()
class SFROUTERACER_API USFResultsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void ShowResults(float CompletionSeconds, float DistanceMeters, int32 RerouteCount);
};
