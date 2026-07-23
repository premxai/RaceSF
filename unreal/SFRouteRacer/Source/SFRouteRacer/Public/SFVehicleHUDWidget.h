#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SFRaceManager.h"
#include "SFVehicleHUDWidget.generated.h"

class UTextureRenderTarget2D;

/**
 * Native race HUD: speedometer, minimap, route panel, timer, countdown.
 */
UCLASS()
class SFROUTERACER_API USFVehicleHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateSpeed(float SpeedKmh);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateGear(int32 Gear);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateRaceTimer(float ElapsedSeconds);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateDestinationDistance(float DistanceMeters);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateDrivenDistance(float DistanceMeters);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void UpdateSuggestedDistance(float DistanceMeters);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetReroutingVisible(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetActiveRouteProfile(const FString& Profile);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetMinimapTexture(UTextureRenderTarget2D* RenderTarget);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetCountdownDisplay(int32 Value);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetRaceStateDisplay(ESFRaceState State);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetDestinationLabel(const FString& Label);

	UFUNCTION(BlueprintCallable, Category = "SF|UI")
	void SetMaxSpeedKmh(float MaxSpeedKmh);

protected:
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	void PaintPanel(
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FGeometry& AllottedGeometry,
		const FVector2D& Pos,
		const FVector2D& Size,
		const FLinearColor& Fill) const;

	void PaintSpeedometer(
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FGeometry& AllottedGeometry,
		const FVector2D& Center,
		float Radius) const;

	void PaintMinimap(
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FGeometry& AllottedGeometry,
		const FVector2D& Pos,
		float Size) const;

	void PaintRoutePanel(
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FGeometry& AllottedGeometry,
		const FVector2D& Pos,
		const FVector2D& Size) const;

	float CachedSpeedKmh = 0.0f;
	float CachedMaxSpeedKmh = 120.0f;
	int32 CachedGear = 1;
	float CachedElapsedSeconds = 0.0f;
	float CachedDestinationMeters = 0.0f;
	float CachedDrivenMeters = 0.0f;
	float CachedSuggestedMeters = 0.0f;
	bool bReroutingVisible = false;
	FString CachedRouteProfile = TEXT("fastest");
	FString CachedDestinationLabel = TEXT("Chase Center");
	int32 CountdownDisplay = -1;
	ESFRaceState CachedRaceState = ESFRaceState::MainMenu;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> MinimapTexture;

	mutable FSlateBrush MinimapBrush;
};
