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
				"DialogueSystemCore"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"AIModule",
				"NavigationSystem"
			}
		);
	}
}
