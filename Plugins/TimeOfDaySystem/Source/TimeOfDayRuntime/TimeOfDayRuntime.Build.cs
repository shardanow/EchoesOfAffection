// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TimeOfDayRuntime : ModuleRules
{
	public TimeOfDayRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"TimeOfDayCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
