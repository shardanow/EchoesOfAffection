// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RelationshipSystemUI : ModuleRules
{
	public RelationshipSystemUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UMG",
				"Slate",
				"SlateCore",
				"GameplayTags",
				"RelationshipSystemCore",
				"GameEventBus" // Add GameEventBus for event-driven UI
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore"
			}
		);
		
		bEnforceIWYU = true;
	}
}
