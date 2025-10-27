// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TimeOfDayUI : ModuleRules
{
	public TimeOfDayUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"Slate",
			"SlateCore",
			"TimeOfDayCore",
			"TimeOfDayRuntime"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
