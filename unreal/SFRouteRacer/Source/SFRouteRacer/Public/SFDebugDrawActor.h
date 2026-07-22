#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFDebugDrawActor.generated.h"

/**
 * Optional debug overlay for graph edges, landmarks, tiles, and destination radius.
 * Disabled automatically in shipping builds.
 */
UCLASS()
class SFROUTERACER_API ASFDebugDrawActor : public AActor
{
	GENERATED_BODY()

public:
	ASFDebugDrawActor();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "SF|Debug")
	void SetDebugEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SF|Debug")
	void SetShowGraph(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SF|Debug")
	void SetShowLandmarks(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SF|Debug")
	void SetShowTiles(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "SF|Debug")
	void SetShowDestinationRadius(bool bEnabled);

protected:
	virtual void BeginPlay() override;

	void DrawGraph() const;
	void DrawLandmarks() const;
	void DrawTiles() const;
	void DrawDestinationRadius() const;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bDebugEnabled = false;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bShowGraph = true;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bShowLandmarks = true;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bShowTiles = false;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	bool bShowDestinationRadius = true;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	float DestinationRadiusMeters = 25.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Debug")
	int32 MaxEdgesToDraw = 400;
};
