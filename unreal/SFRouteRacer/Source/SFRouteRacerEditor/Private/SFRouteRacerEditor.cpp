#include "SFRouteRacerEditor.h"

#include "Framework/Notifications/NotificationManager.h"
#include "LevelEditor.h"
#include "SFMapDataSubsystem.h"
#include "SFMapJsonLoader.h"
#include "ToolMenus.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FSFRouteRacerEditorModule"

void FSFRouteRacerEditorModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSFRouteRacerEditorModule::RegisterMenus));
}

void FSFRouteRacerEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void FSFRouteRacerEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
	FToolMenuSection& Section = Menu->FindOrAddSection("SFRouteRacer");
	Section.AddMenuEntry(
		"ImportSFMapExport",
		LOCTEXT("ImportSFMapExport", "Import SF Route Racer Map Export"),
		LOCTEXT("ImportSFMapExportTooltip", "Validate and load the offline sf_mvp JSON export"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FSFRouteRacerEditorModule::ImportMapExport)));
}

void FSFRouteRacerEditorModule::ImportMapExport()
{
	const FString ExportDirectory = USFMapDataSubsystem::ResolveDefaultExportDirectory();
	FSFMapLoadResult Result;
	FNotificationInfo Info(FText::GetEmpty());
	Info.ExpireDuration = 5.0f;

	if (FSFMapJsonLoader::LoadDirectory(ExportDirectory, Result))
	{
		Info.Text = FText::FromString(FString::Printf(
			TEXT("Loaded SF map export (%d roads, %d buildings, %d races)"),
			Result.Edges.Num(),
			Result.Manifest.BuildingCount,
			Result.Races.Num()));
	}
	else
	{
		Info.Text = FText::FromString(FString::Printf(TEXT("SF map import failed: %s"), *Result.ErrorMessage));
	}

	FSlateNotificationManager::Get().AddNotification(Info);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSFRouteRacerEditorModule, SFRouteRacerEditor)
