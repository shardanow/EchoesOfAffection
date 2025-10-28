// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ScheduleRuntime : ModuleRules
{
	public ScheduleRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"AIModule",
				"NavigationSystem",
				"GameplayTags",
				"ScheduleCore"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
