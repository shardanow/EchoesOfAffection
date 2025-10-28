// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ScheduleCore : ModuleRules
{
	public ScheduleCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"DeveloperSettings"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
