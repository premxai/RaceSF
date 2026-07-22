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

	Manager->SetRaceState(ESFRaceState::RaceSelection);
	TestEqual(TEXT("RaceSelection"), Manager->GetRaceState(), ESFRaceState::RaceSelection);

	Manager->SetRaceState(ESFRaceState::MapLoading);
	Manager->SetRaceState(ESFRaceState::VehicleSpawn);
	Manager->SetRaceState(ESFRaceState::Countdown);
	TestEqual(TEXT("Countdown"), Manager->GetRaceState(), ESFRaceState::Countdown);

	Manager->SetRaceState(ESFRaceState::Racing);
	Manager->SetRaceState(ESFRaceState::DestinationReached);
	Manager->SetRaceState(ESFRaceState::Results);
	TestEqual(TEXT("Results"), Manager->GetRaceState(), ESFRaceState::Results);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFRaceCompletionMetricsTest,
	"SFRouteRacer.Race.CompletionDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFRaceCompletionMetricsTest::RunTest(const FString& Parameters)
{
	ASFRaceManager* Manager = NewObject<ASFRaceManager>();
	TestEqual(TEXT("Initial elapsed"), Manager->GetElapsedRaceSeconds(), 0.0f);
	TestEqual(TEXT("Initial distance"), Manager->GetDistanceDrivenMeters(), 0.0f);
	TestEqual(TEXT("Initial reroutes"), Manager->GetRerouteCount(), 0);
	Manager->NotifyReroute();
	TestEqual(TEXT("Reroute ignored outside racing"), Manager->GetRerouteCount(), 0);
	Manager->SetRaceState(ESFRaceState::Racing);
	Manager->NotifyReroute();
	TestEqual(TEXT("Reroute counted while racing"), Manager->GetRerouteCount(), 1);
	return true;
}

#endif
