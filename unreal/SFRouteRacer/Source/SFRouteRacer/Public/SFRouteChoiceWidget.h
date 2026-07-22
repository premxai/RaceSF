#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFRouteChoiceWidget.generated.h"

USTRUCT(BlueprintType)
struct FSFRouteCardView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SF|UI")
	FString Profile;

	UPROPERTY(BlueprintReadOnly, Category = "SF|UI")
	float DistanceMeters = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SF|UI")
	float EstimatedTimeSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "SF|UI")
	float DeltaSecondsFromFastest = 0.0f;
};

UCLASS()
class SFROUTERACER_API USFRouteChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "SF|UI")
	void ShowRouteCards(const TArray<FSFRouteCardView>& Cards);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void BuildCardsForRace(const FString& RaceId, TArray<FSFRouteCardView>& OutCards);
};
