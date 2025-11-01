// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions/AssetTypeActions_QuestAsset.h"
#include "QuestAsset.h"
#include "ToolMenuSection.h"
#include "Misc/MessageDialog.h"
#include "EditorFramework/AssetImportData.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_QuestAsset::GetName() const
{
	return LOCTEXT("AssetTypeActions_QuestAsset", "Quest Asset");
}

FColor FAssetTypeActions_QuestAsset::GetTypeColor() const
{
	return FColor(255, 196, 128); // Orange
}

UClass* FAssetTypeActions_QuestAsset::GetSupportedClass() const
{
	return UQuestAsset::StaticClass();
}

uint32 FAssetTypeActions_QuestAsset::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}

void FAssetTypeActions_QuestAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// Use default property editor for now
	// In a full implementation, this would open a custom graph-based editor
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

void FAssetTypeActions_QuestAsset::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	TArray<TWeakObjectPtr<UQuestAsset>> QuestAssets = GetTypedWeakObjectPtrs<UQuestAsset>(InObjects);

	Section.AddMenuEntry(
		"QuestAsset_Validate",
		LOCTEXT("QuestAsset_Validate", "Validate Quest Data"),
		LOCTEXT("QuestAsset_ValidateTooltip", "Validates quest data for errors and warnings"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_QuestAsset::ExecuteValidate, QuestAssets),
			FCanExecuteAction()
		)
	);

	Section.AddMenuEntry(
		"QuestAsset_Duplicate",
		LOCTEXT("QuestAsset_Duplicate", "Duplicate with New ID"),
		LOCTEXT("QuestAsset_DuplicateTooltip", "Creates a duplicate of this quest with a new unique ID"),
		FSlateIcon(),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_QuestAsset::ExecuteDuplicateWithNewId, QuestAssets),
			FCanExecuteAction()
		)
	);
}

void FAssetTypeActions_QuestAsset::ExecuteValidate(TArray<TWeakObjectPtr<UQuestAsset>> Objects)
{
	for (auto& ObjPtr : Objects)
	{
		if (UQuestAsset* QuestAsset = ObjPtr.Get())
		{
			TArray<FText> Errors;
			bool bIsValid = QuestAsset->ValidateQuestData(Errors);

			FString Message;
			if (bIsValid)
			{
				Message = FString::Printf(TEXT("Quest '%s' is valid!"), *QuestAsset->QuestId.ToString());
			}
			else
			{
				Message = FString::Printf(TEXT("Quest '%s' has %d error(s):\n\n"), 
					*QuestAsset->QuestId.ToString(), Errors.Num());
				
				for (const FText& Error : Errors)
				{
					Message += TEXT("- ") + Error.ToString() + TEXT("\n");
				}
			}

			FMessageDialog::Open(
				bIsValid ? EAppMsgType::Ok : EAppMsgType::Ok,
				FText::FromString(Message)
			);
		}
	}
}

void FAssetTypeActions_QuestAsset::ExecuteDuplicateWithNewId(TArray<TWeakObjectPtr<UQuestAsset>> Objects)
{
	// This would implement duplication logic
	// For now, just show a message
	FMessageDialog::Open(
		EAppMsgType::Ok,
		LOCTEXT("DuplicateNotImplemented", "Duplicate with New ID - Not yet implemented.\nUse standard duplicate and manually change Quest ID.")
	);
}

#undef LOCTEXT_NAMESPACE
