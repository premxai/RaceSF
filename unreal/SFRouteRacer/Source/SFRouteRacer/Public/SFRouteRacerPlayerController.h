#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SFRouteRacerPlayerController.generated.h"

/**
 * Player controller for vehicle possession and future race UI.
 */
UCLASS()
class SFROUTERACER_API ASFRouteRacerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASFRouteRacerPlayerController();

protected:
	virtual void BeginPlay() override;
};
