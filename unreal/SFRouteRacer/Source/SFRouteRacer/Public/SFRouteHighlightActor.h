#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFRouteHighlightActor.generated.h"

class UProceduralMeshComponent;

/**
 * Draws the currently selected / active navigation route in world space.
 */
UCLASS()
class SFROUTERACER_API ASFRouteHighlightActor : public AActor
{
	GENERATED_BODY()

public:
	ASFRouteHighlightActor();

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void SetHighlightedEdgeIds(const TArray<FString>& EdgeIds, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void HighlightProfileRoute(const TArray<FString>& EdgeIds, const FString& Profile);

	UFUNCTION(BlueprintCallable, Category = "SF|Navigation")
	void ClearHighlight();

	UFUNCTION(BlueprintPure, Category = "SF|Navigation")
	static FLinearColor ColorForProfile(const FString& Profile);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SF|Navigation")
	TObjectPtr<UProceduralMeshComponent> HighlightMesh;

	UPROPERTY(EditAnywhere, Category = "SF|Navigation")
	float HighlightWidthMeters = 6.0f;

	UPROPERTY(EditAnywhere, Category = "SF|Navigation")
	float HeightOffsetCm = 40.0f;
};
