#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFCreditsWidget.generated.h"

UCLASS()
class SFROUTERACER_API USFCreditsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	TArray<FString> GetAttributionLines() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void ShowCredits(const TArray<FString>& Lines);
};
