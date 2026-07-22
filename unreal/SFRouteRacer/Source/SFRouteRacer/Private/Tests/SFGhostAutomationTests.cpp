#include "Misc/AutomationTest.h"
#include "SFGhostOpponentActor.h"
#include "SFRouteHighlightActor.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFGhostOpponentDefaultsTest,
	"SFRouteRacer.AI.GhostDefaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFGhostOpponentDefaultsTest::RunTest(const FString& Parameters)
{
	ASFGhostOpponentActor* Ghost = NewObject<ASFGhostOpponentActor>();
	TestNotNull(TEXT("Ghost created"), Ghost);
	TestFalse(TEXT("Not racing by default"), Ghost->IsRacing());
	TestEqual(TEXT("Progress starts at zero"), Ghost->GetProgressAlpha(), 0.0f);
	Ghost->StopGhost();
	TestFalse(TEXT("Still not racing"), Ghost->IsRacing());
	return true;
}

#endif
