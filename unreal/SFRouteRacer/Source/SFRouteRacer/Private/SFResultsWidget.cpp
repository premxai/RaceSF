#include "SFResultsWidget.h"

#include "Fonts/SlateFontInfo.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"

void USFResultsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

void USFResultsWidget::ShowResults(
	float CompletionSeconds,
	float DistanceMeters,
	float SuggestedDistanceMeters,
	const FString& SelectedRouteProfile,
	int32 RerouteCount,
	float BestTimeSeconds)
{
	CachedCompletionSeconds = CompletionSeconds;
	CachedDistanceMeters = DistanceMeters;
	CachedSuggestedDistanceMeters = SuggestedDistanceMeters;
	CachedRouteProfile = SelectedRouteProfile;
	CachedRerouteCount = RerouteCount;
	CachedBestTimeSeconds = BestTimeSeconds;
	bVisibleResults = true;
	SetVisibility(ESlateVisibility::Visible);
	SetKeyboardFocus();
}

void USFResultsWidget::HideResults()
{
	bVisibleResults = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

FReply USFResultsWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bVisibleResults)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	if (InKeyEvent.GetKey() == EKeys::R || InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		OnRestartRequested.Broadcast();
		return FReply::Handled();
	}
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::BackSpace)
	{
		OnReturnRequested.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

int32 USFResultsWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	LayerId = Super::NativePaint(
		Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (!bVisibleResults)
	{
		return LayerId;
	}

	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		WhiteBrush,
		ESlateDrawEffect::None,
		FLinearColor(0.02f, 0.03f, 0.05f, 0.72f));

	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 36);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 20);
	const FLinearColor TitleColor(1.0f, 0.55f, 0.12f, 1.0f);
	const FLinearColor TextColor(0.95f, 0.95f, 0.92f, 1.0f);

	auto DrawText = [&](const FString& Text, float Y, const FSlateFontInfo& Font, const FLinearColor& Color)
	{
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2f(480.0f, 48.0f), FSlateLayoutTransform(FVector2f(Size.X * 0.5f - 220.0f, Y))),
			Text,
			Font,
			ESlateDrawEffect::None,
			Color);
	};

	const int32 Total = FMath::Max(0, FMath::FloorToInt(CachedCompletionSeconds));
	const FString TimeText = FString::Printf(
		TEXT("%d:%02d.%02d"),
		Total / 60,
		Total % 60,
		FMath::Clamp(FMath::FloorToInt(FMath::Fractional(CachedCompletionSeconds) * 100.0f), 0, 99));

	DrawText(TEXT("DESTINATION REACHED"), Size.Y * 0.28f, TitleFont, TitleColor);
	DrawText(FString::Printf(TEXT("Time  %s"), *TimeText), Size.Y * 0.38f, BodyFont, TextColor);
	if (CachedBestTimeSeconds > 0.0f)
	{
		const int32 BestTotal = FMath::FloorToInt(CachedBestTimeSeconds);
		DrawText(
			FString::Printf(TEXT("Best  %d:%02d.%02d"), BestTotal / 60, BestTotal % 60,
				FMath::Clamp(FMath::FloorToInt(FMath::Fractional(CachedBestTimeSeconds) * 100.0f), 0, 99)),
			Size.Y * 0.43f,
			BodyFont,
			TextColor);
	}
	DrawText(
		FString::Printf(TEXT("Driven  %.0f m   ·   Suggested  %.0f m"), CachedDistanceMeters, CachedSuggestedDistanceMeters),
		Size.Y * 0.49f,
		BodyFont,
		TextColor);
	DrawText(
		FString::Printf(TEXT("Route  %s   ·   Reroutes  %d"), *CachedRouteProfile.ToUpper(), CachedRerouteCount),
		Size.Y * 0.54f,
		BodyFont,
		TextColor);
	DrawText(TEXT("R / Enter  Restart     Esc  Race select"), Size.Y * 0.64f, BodyFont, TitleColor);

	return LayerId + 2;
}
