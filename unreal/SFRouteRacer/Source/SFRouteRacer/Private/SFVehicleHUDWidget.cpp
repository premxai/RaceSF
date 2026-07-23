#include "SFVehicleHUDWidget.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Fonts/SlateFontInfo.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateBrush.h"

namespace
{
FString FormatRaceTime(float Seconds)
{
	const int32 Total = FMath::Max(0, FMath::FloorToInt(Seconds));
	const int32 Minutes = Total / 60;
	const int32 Secs = Total % 60;
	const int32 Hundredths = FMath::Clamp(FMath::FloorToInt(FMath::Fractional(Seconds) * 100.0f), 0, 99);
	return FString::Printf(TEXT("%d:%02d.%02d"), Minutes, Secs, Hundredths);
}

FVector2f Polar(const FVector2D& Center, float AngleRadians, float Radius)
{
	return FVector2f(
		Center.X + FMath::Cos(AngleRadians) * Radius,
		Center.Y + FMath::Sin(AngleRadians) * Radius);
}
} // namespace

void USFVehicleHUDWidget::UpdateSpeed(float SpeedKmh)
{
	CachedSpeedKmh = SpeedKmh;
	if (SpeedKmh < -1.0f)
	{
		CachedGear = -1;
	}
	else if (SpeedKmh < 1.0f)
	{
		CachedGear = 0;
	}
	else
	{
		CachedGear = 1;
	}
}

void USFVehicleHUDWidget::UpdateGear(int32 Gear)
{
	CachedGear = Gear;
}

void USFVehicleHUDWidget::UpdateRaceTimer(float ElapsedSeconds)
{
	CachedElapsedSeconds = ElapsedSeconds;
}

void USFVehicleHUDWidget::UpdateDestinationDistance(float DistanceMeters)
{
	CachedDestinationMeters = DistanceMeters;
}

void USFVehicleHUDWidget::UpdateDrivenDistance(float DistanceMeters)
{
	CachedDrivenMeters = DistanceMeters;
}

void USFVehicleHUDWidget::UpdateSuggestedDistance(float DistanceMeters)
{
	CachedSuggestedMeters = DistanceMeters;
}

void USFVehicleHUDWidget::SetReroutingVisible(bool bVisible)
{
	bReroutingVisible = bVisible;
}

void USFVehicleHUDWidget::SetActiveRouteProfile(const FString& Profile)
{
	CachedRouteProfile = Profile;
}

void USFVehicleHUDWidget::SetMinimapTexture(UTextureRenderTarget2D* RenderTarget)
{
	MinimapTexture = RenderTarget;
	if (MinimapTexture)
	{
		MinimapBrush = FSlateBrush();
		MinimapBrush.SetResourceObject(MinimapTexture);
		MinimapBrush.ImageSize = FVector2D(MinimapTexture->SizeX, MinimapTexture->SizeY);
		MinimapBrush.DrawAs = ESlateBrushDrawType::Image;
		MinimapBrush.Tiling = ESlateBrushTileType::NoTile;
	}
}

void USFVehicleHUDWidget::SetCountdownDisplay(int32 Value)
{
	CountdownDisplay = Value;
}

void USFVehicleHUDWidget::SetRaceStateDisplay(ESFRaceState State)
{
	CachedRaceState = State;
	if (State == ESFRaceState::Racing || State == ESFRaceState::Results)
	{
		CountdownDisplay = -1;
	}
}

void USFVehicleHUDWidget::SetDestinationLabel(const FString& Label)
{
	CachedDestinationLabel = Label;
}

void USFVehicleHUDWidget::SetMaxSpeedKmh(float MaxSpeedKmh)
{
	CachedMaxSpeedKmh = FMath::Max(60.0f, MaxSpeedKmh);
}

void USFVehicleHUDWidget::PaintPanel(
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FGeometry& AllottedGeometry,
	const FVector2D& Pos,
	const FVector2D& Size,
	const FLinearColor& Fill) const
{
	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2f(Size), FSlateLayoutTransform(FVector2f(Pos))),
		WhiteBrush,
		ESlateDrawEffect::None,
		Fill);
}

void USFVehicleHUDWidget::PaintSpeedometer(
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FGeometry& AllottedGeometry,
	const FVector2D& Center,
	float Radius) const
{
	const FSlateFontInfo SpeedFont = FCoreStyle::GetDefaultFontStyle("Bold", 42);
	const FSlateFontInfo LabelFont = FCoreStyle::GetDefaultFontStyle("Bold", 16);
	const FSlateFontInfo UnitFont = FCoreStyle::GetDefaultFontStyle("Regular", 14);
	const FLinearColor DialColor(0.08f, 0.09f, 0.11f, 0.82f);
	const FLinearColor Accent(1.0f, 0.55f, 0.12f, 1.0f);
	const FLinearColor TickColor(0.85f, 0.85f, 0.82f, 0.9f);

	PaintPanel(
		OutDrawElements,
		LayerId,
		AllottedGeometry,
		Center - FVector2D(Radius + 18.0f, Radius + 10.0f),
		FVector2D((Radius + 18.0f) * 2.0f, Radius + 78.0f),
		DialColor);

	// Gauge sweeps from 225° to -45° (left to right through the top).
	constexpr float StartAngle = PI * 0.75f;
	constexpr float EndAngle = -PI * 0.25f;
	const float Sweep = EndAngle - StartAngle;

	TArray<FVector2D> ArcPoints;
	const int32 ArcSegments = 48;
	for (int32 i = 0; i <= ArcSegments; ++i)
	{
		const float T = static_cast<float>(i) / ArcSegments;
		const float Angle = StartAngle + Sweep * T;
		ArcPoints.Add(FVector2D(Polar(Center, Angle, Radius)));
	}
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(),
		ArcPoints,
		ESlateDrawEffect::None,
		Accent,
		true,
		3.0f);

	for (int32 Tick = 0; Tick <= 8; ++Tick)
	{
		const float T = static_cast<float>(Tick) / 8.0f;
		const float Angle = StartAngle + Sweep * T;
		const FVector2D Outer(Polar(Center, Angle, Radius));
		const FVector2D Inner(Polar(Center, Angle, Radius - ((Tick % 2 == 0) ? 16.0f : 10.0f)));
		TArray<FVector2D> TickLine = {Inner, Outer};
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			TickLine,
			ESlateDrawEffect::None,
			TickColor,
			true,
			2.0f);

		if (Tick % 2 == 0)
		{
			const float LabelSpeed = CachedMaxSpeedKmh * T;
			const FVector2f LabelPos = Polar(Center, Angle, Radius - 34.0f) - FVector2f(12.0f, 8.0f);
			FSlateDrawElement::MakeText(
				OutDrawElements,
				LayerId + 2,
				AllottedGeometry.ToPaintGeometry(FVector2f(40.0f, 20.0f), FSlateLayoutTransform(LabelPos)),
				FString::Printf(TEXT("%.0f"), LabelSpeed),
				UnitFont,
				ESlateDrawEffect::None,
				TickColor);
		}
	}

	const float SpeedAlpha = FMath::Clamp(FMath::Abs(CachedSpeedKmh) / CachedMaxSpeedKmh, 0.0f, 1.0f);
	const float NeedleAngle = StartAngle + Sweep * SpeedAlpha;
	const FVector2D NeedleTip(Polar(Center, NeedleAngle, Radius - 22.0f));
	TArray<FVector2D> Needle = {Center, NeedleTip};
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId + 2,
		AllottedGeometry.ToPaintGeometry(),
		Needle,
		ESlateDrawEffect::None,
		Accent,
		true,
		4.0f);

	const FString GearText = (CachedGear < 0) ? TEXT("R") : ((CachedGear == 0) ? TEXT("N") : TEXT("D"));
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 3,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(220.0f, 50.0f), FSlateLayoutTransform(FVector2f(Center.X - 50.0f, Center.Y - 18.0f))),
		FString::Printf(TEXT("%.0f"), FMath::Abs(CachedSpeedKmh)),
		SpeedFont,
		ESlateDrawEffect::None,
		FLinearColor::White);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 3,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(120.0f, 24.0f), FSlateLayoutTransform(FVector2f(Center.X - 28.0f, Center.Y + 28.0f))),
		TEXT("km/h"),
		UnitFont,
		ESlateDrawEffect::None,
		TickColor);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 3,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(40.0f, 24.0f), FSlateLayoutTransform(FVector2f(Center.X + Radius - 36.0f, Center.Y + 8.0f))),
		GearText,
		LabelFont,
		ESlateDrawEffect::None,
		Accent);
}

void USFVehicleHUDWidget::PaintMinimap(
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FGeometry& AllottedGeometry,
	const FVector2D& Pos,
	float Size) const
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 14);
	PaintPanel(
		OutDrawElements,
		LayerId,
		AllottedGeometry,
		Pos - FVector2D(10.0f, 34.0f),
		FVector2D(Size + 20.0f, Size + 54.0f),
		FLinearColor(0.04f, 0.05f, 0.07f, 0.88f));

	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(120.0f, 20.0f), FSlateLayoutTransform(FVector2f(Pos.X, Pos.Y - 26.0f))),
		TEXT("MAP"),
		TitleFont,
		ESlateDrawEffect::None,
		FLinearColor(1.0f, 0.55f, 0.12f, 1.0f));

	const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2f(Size), FSlateLayoutTransform(FVector2f(Pos))),
		WhiteBrush,
		ESlateDrawEffect::None,
		FLinearColor(0.12f, 0.14f, 0.16f, 1.0f));

	if (MinimapTexture)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 2,
			AllottedGeometry.ToPaintGeometry(FVector2f(Size), FSlateLayoutTransform(FVector2f(Pos))),
			&MinimapBrush,
			ESlateDrawEffect::None,
			FLinearColor::White);
	}
	else
	{
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 2,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(Size), FSlateLayoutTransform(FVector2f(Pos.X + 40.0f, Pos.Y + Size * 0.45f))),
			TEXT("Waiting for map…"),
			FCoreStyle::GetDefaultFontStyle("Regular", 14),
			ESlateDrawEffect::None,
			FLinearColor(0.7f, 0.7f, 0.68f, 1.0f));
	}

	// Player chevron at map center (capture follows the pawn).
	const FVector2D MapCenter = Pos + FVector2D(Size * 0.5f, Size * 0.5f);
	TArray<FVector2D> Chevron = {
		MapCenter + FVector2D(0.0f, -12.0f),
		MapCenter + FVector2D(9.0f, 10.0f),
		MapCenter + FVector2D(0.0f, 4.0f),
		MapCenter + FVector2D(-9.0f, 10.0f),
		MapCenter + FVector2D(0.0f, -12.0f)};
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId + 3,
		AllottedGeometry.ToPaintGeometry(),
		Chevron,
		ESlateDrawEffect::None,
		FLinearColor(1.0f, 0.45f, 0.08f, 1.0f),
		true,
		2.5f);
}

void USFVehicleHUDWidget::PaintRoutePanel(
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FGeometry& AllottedGeometry,
	const FVector2D& Pos,
	const FVector2D& Size) const
{
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 16);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Regular", 15);
	const FLinearColor Text(0.94f, 0.94f, 0.91f, 1.0f);
	const FLinearColor Accent(0.15f, 0.85f, 1.0f, 1.0f);

	PaintPanel(OutDrawElements, LayerId, AllottedGeometry, Pos, Size, FLinearColor(0.04f, 0.05f, 0.07f, 0.88f));

	auto DrawLine = [&](const FString& Line, float Y, const FLinearColor& Color)
	{
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(Size.X - 24.0f, 22.0f), FSlateLayoutTransform(FVector2f(Pos.X + 14.0f, Y))),
			Line,
			BodyFont,
			ESlateDrawEffect::None,
			Color);
	};

	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(Size.X - 24.0f, 24.0f), FSlateLayoutTransform(FVector2f(Pos.X + 14.0f, Pos.Y + 12.0f))),
		TEXT("ROUTE"),
		TitleFont,
		ESlateDrawEffect::None,
		Accent);

	DrawLine(CachedRouteProfile.ToUpper(), Pos.Y + 40.0f, FLinearColor(1.0f, 0.55f, 0.12f, 1.0f));
	DrawLine(
		FString::Printf(TEXT("To %s"), *CachedDestinationLabel),
		Pos.Y + 64.0f,
		Text);
	DrawLine(FString::Printf(TEXT("Remaining   %.0f m"), CachedDestinationMeters), Pos.Y + 88.0f, Text);
	DrawLine(FString::Printf(TEXT("Suggested   %.0f m"), CachedSuggestedMeters), Pos.Y + 110.0f, Text);
	DrawLine(FString::Printf(TEXT("Driven      %.0f m"), CachedDrivenMeters), Pos.Y + 132.0f, Text);

	if (bReroutingVisible)
	{
		DrawLine(TEXT("REROUTING…"), Pos.Y + 158.0f, FLinearColor(1.0f, 0.85f, 0.2f, 1.0f));
	}
}

int32 USFVehicleHUDWidget::NativePaint(
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

	const FVector2D Size = AllottedGeometry.GetLocalSize();
	const FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 22);
	const FSlateFontInfo BodyFont = FCoreStyle::GetDefaultFontStyle("Bold", 20);
	const FSlateFontInfo BigFont = FCoreStyle::GetDefaultFontStyle("Bold", 96);
	const FLinearColor Accent(1.0f, 0.55f, 0.12f, 1.0f);
	const FLinearColor Text(0.95f, 0.95f, 0.92f, 1.0f);

	// Top-left race clock.
	PaintPanel(
		OutDrawElements,
		LayerId,
		AllottedGeometry,
		FVector2D(24.0f, 20.0f),
		FVector2D(260.0f, 84.0f),
		FLinearColor(0.04f, 0.05f, 0.07f, 0.88f));
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(240.0f, 24.0f), FSlateLayoutTransform(FVector2f(38.0f, 30.0f))),
		TEXT("RACE TIME"),
		FCoreStyle::GetDefaultFontStyle("Bold", 14),
		ESlateDrawEffect::None,
		Accent);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(240.0f, 36.0f), FSlateLayoutTransform(FVector2f(38.0f, 52.0f))),
		FormatRaceTime(CachedElapsedSeconds),
		BodyFont,
		ESlateDrawEffect::None,
		Text);

	// Bottom-left speedometer.
	const float GaugeRadius = FMath::Clamp(Size.Y * 0.14f, 90.0f, 130.0f);
	PaintSpeedometer(
		OutDrawElements,
		LayerId,
		AllottedGeometry,
		FVector2D(38.0f + GaugeRadius, Size.Y - 40.0f - GaugeRadius * 0.35f),
		GaugeRadius);

	// Right-side minimap + route panel.
	const float MapSize = FMath::Clamp(FMath::Min(Size.X, Size.Y) * 0.28f, 180.0f, 280.0f);
	const FVector2D MapPos(Size.X - MapSize - 28.0f, 36.0f);
	PaintMinimap(OutDrawElements, LayerId, AllottedGeometry, MapPos, MapSize);

	const FVector2D RoutePos(MapPos.X, MapPos.Y + MapSize + 28.0f);
	PaintRoutePanel(
		OutDrawElements, LayerId, AllottedGeometry, RoutePos, FVector2D(MapSize, bReroutingVisible ? 190.0f : 168.0f));

	if (CachedRaceState == ESFRaceState::Countdown && CountdownDisplay >= 0)
	{
		const FString CountdownText = (CountdownDisplay == 0) ? TEXT("GO") : FString::FromInt(CountdownDisplay);
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 5,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(240.0f, 120.0f),
				FSlateLayoutTransform(FVector2f(Size.X * 0.5f - 50.0f, Size.Y * 0.32f))),
			CountdownText,
			BigFont,
			ESlateDrawEffect::None,
			Accent);
		FSlateDrawElement::MakeText(
			OutDrawElements,
			LayerId + 5,
			AllottedGeometry.ToPaintGeometry(
				FVector2f(520.0f, 28.0f),
				FSlateLayoutTransform(FVector2f(Size.X * 0.5f - 210.0f, Size.Y * 0.52f))),
			TEXT("1 / 2 / 3  pick route   ·   wait for GO"),
			FCoreStyle::GetDefaultFontStyle("Regular", 18),
			ESlateDrawEffect::None,
			Text);
	}

	(void)TitleFont;
	return LayerId + 6;
}
