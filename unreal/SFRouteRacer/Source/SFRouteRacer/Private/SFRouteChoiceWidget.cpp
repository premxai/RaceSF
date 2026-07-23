#include "SFRouteChoiceWidget.h"

#include "Engine/GameInstance.h"
#include "Fonts/SlateFontInfo.h"
#include "Rendering/DrawElements.h"
#include "SFMapDataSubsystem.h"
#include "Styling/CoreStyle.h"

void USFRouteChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

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

void USFRouteChoiceWidget::ShowRouteCards(const TArray<FSFRouteCardView>& Cards)
{
	CachedCards = Cards;
	SelectedIndex = 0;
	bShowing = CachedCards.Num() > 0;
	SetVisibility(bShowing ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (bShowing)
	{
		SetKeyboardFocus();
		OnRouteProfileSelected.Broadcast(GetSelectedProfile());
	}
}

void USFRouteChoiceWidget::HideCards()
{
	bShowing = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void USFRouteChoiceWidget::SetSelectedIndex(int32 Index)
{
	if (CachedCards.Num() == 0)
	{
		return;
	}
	SelectedIndex = FMath::Clamp(Index, 0, CachedCards.Num() - 1);
	OnRouteProfileSelected.Broadcast(GetSelectedProfile());
}

FString USFRouteChoiceWidget::GetSelectedProfile() const
{
	if (CachedCards.IsValidIndex(SelectedIndex))
	{
		return CachedCards[SelectedIndex].Profile;
	}
	return TEXT("fastest");
}

FReply USFRouteChoiceWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bShowing)
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::One || Key == EKeys::NumPadOne)
	{
		SetSelectedIndex(0);
		return FReply::Handled();
	}
	if (Key == EKeys::Two || Key == EKeys::NumPadTwo)
	{
		SetSelectedIndex(1);
		return FReply::Handled();
	}
	if (Key == EKeys::Three || Key == EKeys::NumPadThree)
	{
		SetSelectedIndex(2);
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

int32 USFRouteChoiceWidget::NativePaint(
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

	if (!bShowing || CachedCards.Num() == 0)
	{
		return LayerId;
	}

	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 18);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 16);
	const float CardWidth = 200.0f;
	const float Gap = 16.0f;
	const float TotalWidth = CachedCards.Num() * CardWidth + (CachedCards.Num() - 1) * Gap;
	float X = Size.X * 0.5f - TotalWidth * 0.5f;
	const float Y = Size.Y * 0.72f;

	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");

	for (int32 Index = 0; Index < CachedCards.Num(); ++Index)
	{
		const FSFRouteCardView& Card = CachedCards[Index];
		const bool bSelected = Index == SelectedIndex;
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(FVector2f(CardWidth, 88.0f), FSlateLayoutTransform(FVector2f(X, Y))),
			WhiteBrush,
			ESlateDrawEffect::None,
			bSelected ? FLinearColor(1.0f, 0.45f, 0.1f, 0.85f) : FLinearColor(0.05f, 0.06f, 0.08f, 0.75f));

		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2f(CardWidth - 20.0f, 24.0f), FSlateLayoutTransform(FVector2f(X + 14.0f, Y + 12.0f))),
			FString::Printf(TEXT("%d  %s"), Index + 1, *Card.Profile.ToUpper()),
			TitleFont,
			ESlateDrawEffect::None,
			FLinearColor::White);

		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2f(CardWidth - 20.0f, 40.0f), FSlateLayoutTransform(FVector2f(X + 14.0f, Y + 42.0f))),
			FString::Printf(TEXT("%.0f m · %.0fs"), Card.DistanceMeters, Card.EstimatedTimeSeconds),
			BodyFont,
			ESlateDrawEffect::None,
			FLinearColor(0.9f, 0.9f, 0.88f, 1.0f));

		X += CardWidth + Gap;
	}

	return LayerId + 2;
}
