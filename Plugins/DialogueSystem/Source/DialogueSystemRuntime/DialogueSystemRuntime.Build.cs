// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueSystemRuntime : ModuleRules
{
	public DialogueSystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"Json",
				"JsonUtilities",
				"EnhancedInput",
				"DialogueSystemCore",
				"GameEventBus" // For emitting dialogue events to quest system
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AIModule",
				"NavigationSystem"
			}
		);
		
		// Optional dependency on QuestSystem (if plugin is enabled)
		// This allows DialogueSystem to use QuestActorComponent when available
		if (Target.bBuildAllModules || System.IO.Directory.Exists(System.IO.Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "QuestSystem")))
		{
			PrivateDependencyModuleNames.Add("QuestSystemRuntime");
		}
	}
}
