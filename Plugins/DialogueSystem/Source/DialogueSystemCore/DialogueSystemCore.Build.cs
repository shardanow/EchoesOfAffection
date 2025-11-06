// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DialogueSystemCore : ModuleRules
{
	public DialogueSystemCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
		
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
				"LevelSequence",     // For Level Sequence support
				"MovieScene",        // For sequence playback
				"MovieSceneTracks",  // NEW v1.16.8: For UMovieScene3DTransformTrack
				"AIModule",          // For AI movement in positioning
				"NavigationSystem",  // For pathfinding
				"Niagara"         // For VFX effects
			}
		);
		
		// Optional dependency on GameEventBus (if plugin is enabled)
		PrivateDependencyModuleNames.Add("GameEventBus");
		PublicDefinitions.Add("WITH_GAMEEVENTBUS=1");
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
