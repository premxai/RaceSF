using UnrealBuildTool;
using System.Collections.Generic;

public class SFRouteRacerEditorTarget : TargetRules
{
	public SFRouteRacerEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		ExtraModuleNames.AddRange(new[] { "SFRouteRacer", "SFGeoRuntime", "SFRouteRacerEditor" });
	}
}
