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

		// Optional dependency on GameEventBus (soft integration with DialogueSystem)
		if (Target.bBuildAllModules || System.IO.Directory.Exists(System.IO.Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "GameEventBus")))
		{
			PrivateDependencyModuleNames.Add("GameEventBus");
			PublicDefinitions.Add("WITH_GAMEEVENTBUS=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_GAMEEVENTBUS=0");
		}
	}
}
