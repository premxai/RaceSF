#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "SFGeoCoordinateLibrary.h"
#include "SFMapJsonLoader.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFGeoCoordinateConversionTest,
	"SFRouteRacer.Geo.CoordinateConversion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFGeoCoordinateConversionTest::RunTest(const FString& Parameters)
{
	const FVector Unreal = USFGeoCoordinateLibrary::LocalMetersToUnreal(FVector(12.5, 3.0, 1.25));
	TestEqual(TEXT("X cm"), Unreal.X, 1250.0);
	TestEqual(TEXT("Y negated"), Unreal.Y, -300.0);
	TestEqual(TEXT("Z cm"), Unreal.Z, 125.0);

	const FVector RoundTrip = USFGeoCoordinateLibrary::UnrealToLocalMeters(Unreal);
	TestEqual(TEXT("Round trip X"), RoundTrip.X, 12.5);
	TestEqual(TEXT("Round trip Y"), RoundTrip.Y, 3.0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFMapFixtureLoadTest,
	"SFRouteRacer.Geo.LoadFixtureManifest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFMapFixtureLoadTest::RunTest(const FString& Parameters)
{
	const FString FixtureDir = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectContentDir(), TEXT("TestData/sf_mvp_fixture")));

	FSFMapLoadResult Result;
	const bool bLoaded = FSFMapJsonLoader::LoadDirectory(FixtureDir, Result);
	TestTrue(TEXT("Fixture loads"), bLoaded);
	TestEqual(TEXT("Schema"), Result.Manifest.SchemaVersion, FString(TEXT("0.1.0")));
	TestEqual(TEXT("Nodes"), Result.Nodes.Num(), 4);
	TestEqual(TEXT("Edges"), Result.Edges.Num(), 4);
	TestEqual(TEXT("Landmarks"), Result.Landmarks.Num(), 2);
	TestEqual(TEXT("Races"), Result.Races.Num(), 1);
	TestEqual(TEXT("Tiles"), Result.Tiles.Num(), 1);
	return bLoaded;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FSFMapInvalidSchemaTest,
	"SFRouteRacer.Geo.RejectInvalidSchema",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSFMapInvalidSchemaTest::RunTest(const FString& Parameters)
{
	FSFMapManifestData Manifest;
	FString Error;
	const FString TempPath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("sf_invalid_manifest.json"));
	const FString Payload = TEXT("{\"schema_version\":\"9.9.9\",\"projection\":\"EPSG:32610\",\"graph_file\":\"g.json\",\"landmarks_file\":\"l.json\",\"races_file\":\"r.json\",\"tile_files\":[]}");
	FFileHelper::SaveStringToFile(Payload, *TempPath);
	const bool bLoaded = FSFMapJsonLoader::LoadManifest(TempPath, Manifest, Error);
	TestFalse(TEXT("Unsupported schema rejected"), bLoaded);
	TestTrue(TEXT("Error mentions schema"), Error.Contains(TEXT("Unsupported schema")));
	return true;
}

#endif
