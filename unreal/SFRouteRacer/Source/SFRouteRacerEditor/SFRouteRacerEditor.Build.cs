using UnrealBuildTool;

public class SFRouteRacerEditor : ModuleRules
{
	public SFRouteRacerEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"SFGeoRuntime",
			"SFRouteRacer"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"ToolMenus",
			"Json",
			"JsonUtilities",
			"Slate",
			"SlateCore"
		});
	}
}
