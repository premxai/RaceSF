#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFMinimapCaptureActor.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class USpringArmComponent;

/**
 * Orthographic top-down SceneCapture used by the UMG minimap.
 */
UCLASS()
class SFROUTERACER_API ASFMinimapCaptureActor : public AActor
{
	GENERATED_BODY()

public:
	ASFMinimapCaptureActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Minimap")
	void SetFollowTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "SF|Minimap")
	void SetNorthUp(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "SF|Minimap")
	UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	UFUNCTION(BlueprintCallable, Category = "SF|Minimap")
	void SetOrthoWidthMeters(float WidthMeters);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Minimap")
	TObjectPtr<USceneCaptureComponent2D> CaptureComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Minimap")
	TObjectPtr<UTextureRenderTarget2D> RenderTarget;

	UPROPERTY(EditAnywhere, Category = "SF|Minimap")
	float OrthoWidthMeters = 450.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Minimap")
	float CaptureHeightCm = 35000.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Minimap")
	bool bNorthUp = false;

	UPROPERTY()
	TWeakObjectPtr<AActor> FollowTarget;
};
