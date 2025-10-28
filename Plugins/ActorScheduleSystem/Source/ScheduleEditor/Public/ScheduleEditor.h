// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * ScheduleEditor Module
 * Editor customizations, asset factories, and tools
 */
class FScheduleEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Register asset type actions */
	void RegisterAssetTypeActions();

	/** Unregister asset type actions */
	void UnregisterAssetTypeActions();

	/** Register detail customizations */
	void RegisterDetailCustomizations();

	/** Unregister detail customizations */
	void UnregisterDetailCustomizations();

	/** Registered asset type actions */
	TArray<TSharedPtr<class IAssetTypeActions>> CreatedAssetTypeActions;
};
