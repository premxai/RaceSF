#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFResultsWidget.generated.h"

/**
 * Native results overlay: time, distance, reroutes, restart (R).
 */
UCLASS()
class SFROUTERACER_API USFResultsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void ShowResults(
		float CompletionSeconds,
		float DistanceMeters,
		float SuggestedDistanceMeters,
		const FString& SelectedRouteProfile,
		int32 RerouteCount,
		float BestTimeSeconds);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void HideResults();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSFResultsRestartRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSFResultsReturnRequested);

	UPROPERTY(BlueprintAssignable, Category = "SF|UI")
	FSFResultsRestartRequested OnRestartRequested;

	UPROPERTY(BlueprintAssignable, Category = "SF|UI")
	FSFResultsReturnRequested OnReturnRequested;

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

	float CachedCompletionSeconds = 0.0f;
	float CachedDistanceMeters = 0.0f;
	float CachedSuggestedDistanceMeters = 0.0f;
	FString CachedRouteProfile;
	int32 CachedRerouteCount = 0;
	float CachedBestTimeSeconds = -1.0f;
	bool bVisibleResults = false;
};
