// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueSystemCore : ModuleRules
{
	public DialogueSystemCore(ReadOnlyTargetRules Target) : base(Target)
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
				"Json",
				"JsonUtilities"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...
			}
		);
		
		// Optional dependency on GameEventBus (if plugin is enabled)
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
