// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Quest System Editor Module
 * Editor-only functionality for quest authoring and validation
 */
class FQuestSystemEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterAssetTypeActions();
	void UnregisterAssetTypeActions();
	void RegisterDetailCustomizations();
	void UnregisterDetailCustomizations();

	TArray<TSharedPtr<class IAssetTypeActions>> CreatedAssetTypeActions;
};
