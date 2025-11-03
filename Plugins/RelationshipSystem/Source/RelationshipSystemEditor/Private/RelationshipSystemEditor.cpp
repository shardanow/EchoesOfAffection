// Copyright Epic Games, Inc. All Rights Reserved.

#include "RelationshipSystemEditor.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "FRelationshipSystemEditorModule"

void FRelationshipSystemEditorModule::StartupModule()
{
	RegisterAssetTools();
	RegisterCustomizations();
	// Note: Project Settings are now auto-registered via UDeveloperSettings
	// No manual registration needed to avoid duplication
}

void FRelationshipSystemEditorModule::ShutdownModule()
{
	UnregisterCustomizations();
	UnregisterAssetTools();
}

void FRelationshipSystemEditorModule::RegisterAssetTools()
{
	// Asset actions will be registered here
}

void FRelationshipSystemEditorModule::UnregisterAssetTools()
{
	// Cleanup asset actions
}

void FRelationshipSystemEditorModule::RegisterCustomizations()
{
	// Property customizations will be registered here
}

void FRelationshipSystemEditorModule::UnregisterCustomizations()
{
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		// Unregister customizations
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRelationshipSystemEditorModule, RelationshipSystemEditor)
