// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuestSystemRuntime : ModuleRules
{
	public QuestSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
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
				"Slate",
				"SlateCore",
				"UMG"
			}
		);
		
		// Optional dependency on GameEventBus (if plugin is enabled)
		// This allows QuestSystem to work without GameEventBus
		if (Target.bBuildAllModules || System.IO.Directory.Exists(System.IO.Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "GameEventBus")))
		{
			PrivateDependencyModuleNames.Add("GameEventBus");
		}
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
