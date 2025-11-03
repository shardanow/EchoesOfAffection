// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * Editor module for RelationshipSystem plugin
 * Provides asset editors, validators, and development tools
 * 
 * Note: Project Settings are auto-registered via UDeveloperSettings in RelationshipSystemSettings.h
 */
class FRelationshipSystemEditorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Register asset actions and menu extensions */
	void RegisterAssetTools();
	
	/** Unregister asset actions */
	void UnregisterAssetTools();
	
	/** Register custom detail customizations */
	void RegisterCustomizations();
	
	/** Unregister customizations */
	void UnregisterCustomizations();
};
