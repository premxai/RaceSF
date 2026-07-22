#include "Misc/AutomationTest.h"
#include "SFRouteHighlightActor.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFRouteProfileColorTest,
	"SFRouteRacer.Navigation.ProfileColors",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFRouteProfileColorTest::RunTest(const FString& Parameters)
{
	const FLinearColor Fastest = ASFRouteHighlightActor::ColorForProfile(TEXT("fastest"));
	const FLinearColor Balanced = ASFRouteHighlightActor::ColorForProfile(TEXT("balanced"));
	const FLinearColor Scenic = ASFRouteHighlightActor::ColorForProfile(TEXT("scenic"));
	const FLinearColor Explorer = ASFRouteHighlightActor::ColorForProfile(TEXT("explorer"));

	TestTrue(TEXT("Fastest is cyan-ish"), Fastest.B > Fastest.R);
	TestTrue(TEXT("Balanced is green-ish"), Balanced.G > Balanced.R);
	TestTrue(TEXT("Scenic is yellow-ish"), Scenic.R > 0.8f && Scenic.G > 0.7f);
	TestTrue(TEXT("Explorer is purple-ish"), Explorer.B > Explorer.G);
	return true;
}

#endif
