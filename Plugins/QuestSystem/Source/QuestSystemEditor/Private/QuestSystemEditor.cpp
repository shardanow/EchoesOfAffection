// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestSystemEditor.h"
#include "AssetToolsModule.h"
#include "PropertyEditorModule.h"
#include "AssetTypeActions/AssetTypeActions_QuestAsset.h"
#include "Details/QuestAssetDetails.h"
#include "Factories/QuestAssetFactory.h"

#define LOCTEXT_NAMESPACE "FQuestSystemEditorModule"

void FQuestSystemEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
	RegisterDetailCustomizations();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Module started"));
}

void FQuestSystemEditorModule::ShutdownModule()
{
	UnregisterDetailCustomizations();
	UnregisterAssetTypeActions();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Module shutdown"));
}

void FQuestSystemEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	
	TSharedRef<IAssetTypeActions> QuestAssetAction = MakeShareable(new FAssetTypeActions_QuestAsset());
	AssetTools.RegisterAssetTypeActions(QuestAssetAction);
	CreatedAssetTypeActions.Add(QuestAssetAction);
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Registered asset type actions"));
}

void FQuestSystemEditorModule::UnregisterAssetTypeActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& Action : CreatedAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(Action.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Unregistered asset type actions"));
}

void FQuestSystemEditorModule::RegisterDetailCustomizations()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	PropertyModule.RegisterCustomClassLayout(
		"QuestAsset",
		FOnGetDetailCustomizationInstance::CreateStatic(&FQuestAssetDetails::MakeInstance)
	);
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Registered detail customizations"));
}

void FQuestSystemEditorModule::UnregisterDetailCustomizations()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("QuestAsset");
	}
	
	UE_LOG(LogTemp, Log, TEXT("QuestSystemEditor: Unregistered detail customizations"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FQuestSystemEditorModule, QuestSystemEditor)
