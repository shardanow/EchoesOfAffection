// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions/AssetTypeActions_ScheduleEntry.h"
#include "Data/ScheduleEntryData.h"
#include "Toolkits/SimpleAssetEditor.h"

UClass* FAssetTypeActions_ScheduleEntry::GetSupportedClass() const
{
	return UScheduleEntryData::StaticClass();
}

void FAssetTypeActions_ScheduleEntry::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// Use default property editor
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}
