// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RelationshipSystemCore : ModuleRules
{
	public RelationshipSystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// Optimization settings
		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
		
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
				"Json",
				"JsonUtilities"
			}
		);
		
		// Enable IWYU for better compilation times
		bEnforceIWYU = true;
	}
}
