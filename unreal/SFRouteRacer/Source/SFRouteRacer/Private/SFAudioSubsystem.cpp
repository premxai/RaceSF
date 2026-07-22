#include "SFAudioSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "SFRouteRacer.h"
#include "Sound/SoundBase.h"

void USFAudioSubsystem::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void USFAudioSubsystem::PlayCue(USoundBase* Sound, const TCHAR* DebugName) const
{
	if (!Sound)
	{
		UE_LOG(LogSFRace, Verbose, TEXT("Audio placeholder fired: %s (no asset bound)"), DebugName);
		return;
	}
	UGameplayStatics::PlaySound2D(this, Sound, MasterVolume);
}

void USFAudioSubsystem::PlayCountdownBeep()
{
	PlayCue(CountdownSound, TEXT("countdown"));
}

void USFAudioSubsystem::PlayGoCue()
{
	PlayCue(GoSound, TEXT("go"));
}

void USFAudioSubsystem::PlayDestinationReached()
{
	PlayCue(DestinationSound, TEXT("destination"));
}

void USFAudioSubsystem::PlayRerouteCue()
{
	PlayCue(RerouteSound, TEXT("reroute"));
}

void USFAudioSubsystem::PlayUISelect()
{
	PlayCue(UISelectSound, TEXT("ui_select"));
}
