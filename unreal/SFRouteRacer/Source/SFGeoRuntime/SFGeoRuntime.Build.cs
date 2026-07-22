using UnrealBuildTool;

public class SFGeoRuntime : ModuleRules
{
	public SFGeoRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Json",
			"JsonUtilities",
			"ProceduralMeshComponent"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Projects"
		});
	}
}
