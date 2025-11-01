// Copyright Epic Games, Inc. All Rights Reserved.

#include "Details/QuestAssetDetails.h"
#include "QuestAsset.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/MessageDialog.h"
#include "Misc/Guid.h"

#define LOCTEXT_NAMESPACE "QuestAssetDetails"

TSharedRef<IDetailCustomization> FQuestAssetDetails::MakeInstance()
{
	return MakeShareable(new FQuestAssetDetails);
}

void FQuestAssetDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	if (ObjectsBeingCustomized.Num() > 0)
	{
		QuestAssetPtr = Cast<UQuestAsset>(ObjectsBeingCustomized[0].Get());
	}

	// Add custom category with buttons
	IDetailCategoryBuilder& QuestCategory = DetailBuilder.EditCategory("Quest Tools", FText::GetEmpty(), ECategoryPriority::Important);

	// Validate button
	QuestCategory.AddCustomRow(LOCTEXT("ValidateRow", "Validate"))
		.WholeRowContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ValidateButton", "Validate Quest Data"))
				.ToolTipText(LOCTEXT("ValidateButtonTooltip", "Check quest data for errors and warnings"))
				.OnClicked(this, &FQuestAssetDetails::OnValidateClicked)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("GenerateIdButton", "Generate New Quest ID"))
				.ToolTipText(LOCTEXT("GenerateIdButtonTooltip", "Generate a unique Quest ID based on GUID"))
				.OnClicked(this, &FQuestAssetDetails::OnGenerateQuestIdClicked)
			]
		];

	// Add info text
	if (UQuestAsset* QuestAsset = QuestAssetPtr.Get())
	{
		FText InfoText = FText::Format(
			LOCTEXT("QuestInfo", "Quest ID: {0}\nPhases: {1}\nTotal Objectives: {2}"),
			FText::FromName(QuestAsset->QuestId),
			FText::AsNumber(QuestAsset->Phases.Num()),
			FText::AsNumber(QuestAsset->GetAllObjectiveIds().Num())
		);

		QuestCategory.AddCustomRow(LOCTEXT("InfoRow", "Info"))
			.WholeRowContent()
			[
				SNew(STextBlock)
				.Text(InfoText)
				.Font(IDetailLayoutBuilder::GetDetailFont())
			];
	}
}

FReply FQuestAssetDetails::OnValidateClicked()
{
	if (UQuestAsset* QuestAsset = QuestAssetPtr.Get())
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

	return FReply::Handled();
}

FReply FQuestAssetDetails::OnGenerateQuestIdClicked()
{
	if (UQuestAsset* QuestAsset = QuestAssetPtr.Get())
	{
		// Generate new ID from GUID
		FGuid NewGuid = FGuid::NewGuid();
		FString NewIdString = FString::Printf(TEXT("Q_%s"), *NewGuid.ToString(EGuidFormats::Short));
		FName NewId = FName(*NewIdString);

		// Confirm with user
		FText Message = FText::Format(
			LOCTEXT("GenerateIdConfirm", "Generate new Quest ID?\n\nOld: {0}\nNew: {1}\n\nThis will update the Quest ID. Make sure to update any references!"),
			FText::FromName(QuestAsset->QuestId),
			FText::FromName(NewId)
		);

		EAppReturnType::Type Result = FMessageDialog::Open(EAppMsgType::YesNo, Message);
		
		if (Result == EAppReturnType::Yes)
		{
			QuestAsset->Modify();
			QuestAsset->QuestId = NewId;
			QuestAsset->MarkPackageDirty();

			FMessageDialog::Open(
				EAppMsgType::Ok,
				LOCTEXT("GenerateIdSuccess", "Quest ID generated successfully!")
			);
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
