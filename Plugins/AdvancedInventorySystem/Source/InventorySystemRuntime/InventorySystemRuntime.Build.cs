// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class InventorySystemRuntime : ModuleRules
{
	public InventorySystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"GameplayTags",
				"InventorySystemCore"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Json",
				"JsonUtilities"
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
