// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RelationshipSystemEditor : ModuleRules
{
	public RelationshipSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RelationshipSystemCore"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"Slate",
				"SlateCore",
				"PropertyEditor",
				"EditorStyle",
				"EditorWidgets",
				"AssetTools",
				"AssetRegistry",
				"ContentBrowser",
				"WorkspaceMenuStructure",
				"ToolMenus",
				"DeveloperSettings"
			}
		);
		
		bEnforceIWYU = true;
	}
}
