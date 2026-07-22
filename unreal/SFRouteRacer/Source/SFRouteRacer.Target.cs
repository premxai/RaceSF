using UnrealBuildTool;
using System.Collections.Generic;

public class SFRouteRacerTarget : TargetRules
{
	public SFRouteRacerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new[] { "SFRouteRacer", "SFGeoRuntime" });
	}
}
