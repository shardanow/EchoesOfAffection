// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ScheduleEditor : ModuleRules
{
	public ScheduleEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"PropertyEditor",
				"DetailCustomizations",
				"AssetTools",
				"ToolMenus",
				"GameplayTags",
				"GameplayTagsEditor",
				"ScheduleCore",
				"ScheduleRuntime"
			}
		);
	}
}
