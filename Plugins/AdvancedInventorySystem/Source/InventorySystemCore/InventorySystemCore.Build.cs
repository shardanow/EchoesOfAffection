// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class InventorySystemCore : ModuleRules
{
	public InventorySystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"Json",
				"JsonUtilities"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);

		// Optional GameEventBus integration
		string GameEventBusPath = Path.Combine(Target.ProjectFile.Directory.FullName, "Plugins", "GameEventBus");
		if (Directory.Exists(GameEventBusPath))
		{
			PrivateDependencyModuleNames.Add("GameEventBus");
			PublicDefinitions.Add("GAMEEVENTBUS_AVAILABLE=1");
		}
	}
}
