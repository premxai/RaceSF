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

/**
 * Lightweight route cards during countdown — keys 1/2/3 select a profile.
 */
UCLASS()
class SFROUTERACER_API USFRouteChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void ShowRouteCards(const TArray<FSFRouteCardView>& Cards);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void BuildCardsForRace(const FString& RaceId, TArray<FSFRouteCardView>& OutCards);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetSelectedIndex(int32 Index);

	UFUNCTION(BlueprintPure, Category = "SF|UI")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintPure, Category = "SF|UI")
	FString GetSelectedProfile() const;

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void HideCards();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSFRouteProfileSelected, const FString&, Profile);

	UPROPERTY(BlueprintAssignable, Category = "SF|UI")
	FSFRouteProfileSelected OnRouteProfileSelected;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	UPROPERTY()
	TArray<FSFRouteCardView> CachedCards;

	int32 SelectedIndex = 0;
	bool bShowing = false;
};
