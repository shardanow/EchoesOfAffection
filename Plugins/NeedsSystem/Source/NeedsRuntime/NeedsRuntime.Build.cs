// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NeedsRuntime : ModuleRules
{
	public NeedsRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"NeedsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});

		// Optional dependency on Time System (can work without it)
		if (Target.bBuildAllModules || 
		    System.IO.Directory.Exists(System.IO.Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins/TimeOfDaySystem")))
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"TimeOfDayCore",
				"TimeOfDayRuntime"
			});
			
			PublicDefinitions.Add("WITH_TIME_SYSTEM=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_TIME_SYSTEM=0");
		}
	}
}
