#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SFAudioSubsystem.generated.h"

class USoundBase;

/**
 * Placeholder audio interface for MVP cues. Bind licensed clips in-editor.
 */
UCLASS()
class SFROUTERACER_API USFAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void PlayCountdownBeep();

	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void PlayGoCue();

	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void PlayDestinationReached();

	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void PlayRerouteCue();

	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void PlayUISelect();

	UFUNCTION(BlueprintCallable, Category = "SF|Audio")
	void SetMasterVolume(float Volume);

protected:
	void PlayCue(USoundBase* Sound, const TCHAR* DebugName) const;

	UPROPERTY(EditAnywhere, Category = "SF|Audio")
	TObjectPtr<USoundBase> CountdownSound;

	UPROPERTY(EditAnywhere, Category = "SF|Audio")
	TObjectPtr<USoundBase> GoSound;

	UPROPERTY(EditAnywhere, Category = "SF|Audio")
	TObjectPtr<USoundBase> DestinationSound;

	UPROPERTY(EditAnywhere, Category = "SF|Audio")
	TObjectPtr<USoundBase> RerouteSound;

	UPROPERTY(EditAnywhere, Category = "SF|Audio")
	TObjectPtr<USoundBase> UISelectSound;

	UPROPERTY()
	float MasterVolume = 1.0f;
};
