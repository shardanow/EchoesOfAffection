// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NeedsUI : ModuleRules
{
	public NeedsUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"Slate",
			"SlateCore",
			"GameplayTags",
			"NeedsCore",
			"NeedsRuntime"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
