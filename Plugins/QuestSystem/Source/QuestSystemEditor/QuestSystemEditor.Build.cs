// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuestSystemEditor : ModuleRules
{
	public QuestSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"QuestSystemRuntime",
				"GameplayTags"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				"EditorStyle",
				"UnrealEd",
				"AssetTools",
				"AssetRegistry",
				"PropertyEditor",
				"ContentBrowser",
				"BlueprintGraph",
				"Kismet",
				"KismetCompiler",
				"GameplayTagsEditor",
				"EditorSubsystem",
				"ToolMenus",
				"DeveloperSettings"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
		);
	}
}
