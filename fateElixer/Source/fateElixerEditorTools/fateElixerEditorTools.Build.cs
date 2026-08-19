// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class fateElixerEditorTools : ModuleRules
{
	public fateElixerEditorTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });
		PrivateDependencyModuleNames.AddRange(new string[] { "PythonScriptPlugin", "ToolsetRegistry" });
	}
}
