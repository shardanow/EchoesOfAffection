// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions/AssetTypeActions_ScheduleData.h"
#include "Data/ScheduleData.h"
#include "Data/ScheduleEntryData.h"
#include "ToolMenus.h"
#include "Styling/AppStyle.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/Factory.h"
#include "Misc/MessageDialog.h"
#include "Toolkits/SimpleAssetEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_ScheduleData::GetSupportedClass() const
{
	return UScheduleData::StaticClass();
}

void FAssetTypeActions_ScheduleData::GetActions(const TArray<UObject*>& InObjects, FToolMenuSection& Section)
{
	TArray<TWeakObjectPtr<UScheduleData>> Schedules = GetTypedWeakObjectPtrs<UScheduleData>(InObjects);

	Section.AddMenuEntry(
		"ScheduleData_CreateEntry",
		LOCTEXT("ScheduleData_CreateEntry", "Create Schedule Entry"),
		LOCTEXT("ScheduleData_CreateEntryTooltip", "Creates a new schedule entry asset for this schedule"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ClassIcon.Blueprint"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ScheduleData::ExecuteCreateEntry, Schedules),
			FCanExecuteAction()
		)
	);

	Section.AddMenuEntry(
		"ScheduleData_Validate",
		LOCTEXT("ScheduleData_Validate", "Validate Schedule"),
		LOCTEXT("ScheduleData_ValidateTooltip", "Validates the schedule data for errors"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Check"),
		FUIAction(
			FExecuteAction::CreateSP(this, &FAssetTypeActions_ScheduleData::ExecuteValidateSchedule, Schedules),
			FCanExecuteAction()
		)
	);
}

void FAssetTypeActions_ScheduleData::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	// Use default property editor
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

void FAssetTypeActions_ScheduleData::ExecuteCreateEntry(TArray<TWeakObjectPtr<UScheduleData>> Objects)
{
	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UScheduleData* Schedule = (*ObjIt).Get())
		{
			// Create a new schedule entry asset
			FString PackageName = Schedule->GetOutermost()->GetName();
			FString AssetName = FString::Printf(TEXT("%s_Entry"), *Schedule->GetName());

			// This is a simplified version - in production you'd use proper factory
			UE_LOG(LogTemp, Log, TEXT("Create entry for schedule: %s"), *Schedule->GetName());
		}
	}
}

void FAssetTypeActions_ScheduleData::ExecuteValidateSchedule(TArray<TWeakObjectPtr<UScheduleData>> Objects)
{
	int32 ValidCount = 0;
	int32 InvalidCount = 0;

	for (auto ObjIt = Objects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UScheduleData* Schedule = (*ObjIt).Get())
		{
			if (Schedule->Entries.Num() == 0)
			{
				InvalidCount++;
				UE_LOG(LogTemp, Warning, TEXT("Schedule '%s' has no entries"), *Schedule->GetName());
			}
			else
			{
				ValidCount++;
			}
		}
	}

	FText Message = FText::Format(
		LOCTEXT("ValidationResult", "Validation Complete:\nValid: {0}\nInvalid: {1}"),
		FText::AsNumber(ValidCount),
		FText::AsNumber(InvalidCount)
	);

	FMessageDialog::Open(EAppMsgType::Ok, Message);
}

#undef LOCTEXT_NAMESPACE
