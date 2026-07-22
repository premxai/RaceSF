#include "Misc/AutomationTest.h"
#include "SFRaceManager.h"
#include "SFRaceSubsystem.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFRaceStateTransitionTest,
	"SFRouteRacer.Race.StateTransitions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFRaceStateTransitionTest::RunTest(const FString& Parameters)
{
	ASFRaceManager* Manager = NewObject<ASFRaceManager>();
	TestNotNull(TEXT("Race manager created"), Manager);
	Manager->SetRaceState(ESFRaceState::Loading);
	TestEqual(TEXT("Loading"), Manager->GetRaceState(), ESFRaceState::Loading);
	Manager->SetRaceState(ESFRaceState::Countdown);
	Manager->SetRaceState(ESFRaceState::Racing);
	Manager->SetRaceState(ESFRaceState::Finished);
	TestEqual(TEXT("Finished"), Manager->GetRaceState(), ESFRaceState::Finished);
	return true;
}

#endif
