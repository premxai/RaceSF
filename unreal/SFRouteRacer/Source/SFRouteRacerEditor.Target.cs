using UnrealBuildTool;
using System.Collections.Generic;

public class SFRouteRacerEditorTarget : TargetRules
{
	public SFRouteRacerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange(new[] { "SFRouteRacer", "SFGeoRuntime", "SFRouteRacerEditor" });
	}
}
