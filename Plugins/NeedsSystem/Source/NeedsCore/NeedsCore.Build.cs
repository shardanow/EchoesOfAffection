// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NeedsCore : ModuleRules
{
	public NeedsCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
