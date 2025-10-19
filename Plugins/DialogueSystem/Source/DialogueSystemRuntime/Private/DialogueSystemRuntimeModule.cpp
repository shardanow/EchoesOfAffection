// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

/**
 * Dialogue System Runtime Module Implementation
 */
class FDialogueSystemRuntimeModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
		// Module initialization logic can go here
	}

	virtual void ShutdownModule() override
	{
		// Module shutdown logic can go here
	}
};

IMPLEMENT_MODULE(FDialogueSystemRuntimeModule, DialogueSystemRuntime)
