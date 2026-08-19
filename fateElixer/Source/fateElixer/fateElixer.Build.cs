// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class fateElixer : ModuleRules
{
	public fateElixer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "GameplayTasks", "UMG", "Slate", "SlateCore" });
	}
}
