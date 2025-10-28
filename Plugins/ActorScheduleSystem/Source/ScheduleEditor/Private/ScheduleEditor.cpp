// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScheduleEditor.h"
#include "AssetToolsModule.h"
#include "PropertyEditorModule.h"
#include "AssetTypeActions/AssetTypeActions_ScheduleData.h"
#include "AssetTypeActions/AssetTypeActions_ScheduleEntry.h"
#include "Customizations/ScheduleEntryDataCustomization.h"
#include "Customizations/ScheduleDataCustomization.h"

#define LOCTEXT_NAMESPACE "FScheduleEditorModule"

void FScheduleEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
	RegisterDetailCustomizations();
}

void FScheduleEditorModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
	UnregisterDetailCustomizations();
}

void FScheduleEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Register ScheduleData asset type
	TSharedPtr<IAssetTypeActions> ScheduleDataAction = MakeShared<FAssetTypeActions_ScheduleData>();
	AssetTools.RegisterAssetTypeActions(ScheduleDataAction.ToSharedRef());
	CreatedAssetTypeActions.Add(ScheduleDataAction);

	// Register ScheduleEntryData asset type
	TSharedPtr<IAssetTypeActions> ScheduleEntryAction = MakeShared<FAssetTypeActions_ScheduleEntry>();
	AssetTools.RegisterAssetTypeActions(ScheduleEntryAction.ToSharedRef());
	CreatedAssetTypeActions.Add(ScheduleEntryAction);
}

void FScheduleEditorModule::UnregisterAssetTypeActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (TSharedPtr<IAssetTypeActions> Action : CreatedAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

void FScheduleEditorModule::RegisterDetailCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	// Register custom detail panels
	PropertyModule.RegisterCustomClassLayout(
		"ScheduleEntryData",
		FOnGetDetailCustomizationInstance::CreateStatic(&FScheduleEntryDataCustomization::MakeInstance)
	);

	PropertyModule.RegisterCustomClassLayout(
		"ScheduleData",
		FOnGetDetailCustomizationInstance::CreateStatic(&FScheduleDataCustomization::MakeInstance)
	);

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FScheduleEditorModule::UnregisterDetailCustomizations()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		
		PropertyModule.UnregisterCustomClassLayout("ScheduleEntryData");
		PropertyModule.UnregisterCustomClassLayout("ScheduleData");

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FScheduleEditorModule, ScheduleEditor)
