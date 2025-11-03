// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRelationshipSystem, Log, All);

/**
 * Core runtime module for RelationshipSystem plugin
 * Handles initialization, shutdown, and module lifecycle
 */
class FRelationshipSystemCoreModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** Check if module is loaded and ready */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RelationshipSystemCore");
	}
	
	/** Get module instance */
	static inline FRelationshipSystemCoreModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FRelationshipSystemCoreModule>("RelationshipSystemCore");
	}
};
