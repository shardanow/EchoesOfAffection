// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueSystemAI : ModuleRules
{
	public DialogueSystemAI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"HTTP",
				"Json",
				"JsonUtilities",
				"GameplayTags",
				"DialogueSystemCore"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Sockets",
				"Networking"
			}
		);
	}
}
