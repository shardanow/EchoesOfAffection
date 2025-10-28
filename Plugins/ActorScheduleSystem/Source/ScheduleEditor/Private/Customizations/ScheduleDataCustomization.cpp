// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/ScheduleDataCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Data/ScheduleData.h"

#define LOCTEXT_NAMESPACE "ScheduleDataCustomization"

TSharedRef<IDetailCustomization> FScheduleDataCustomization::MakeInstance()
{
	return MakeShareable(new FScheduleDataCustomization);
}

void FScheduleDataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedDetailBuilder = &DetailBuilder;

	// Reorganize categories
	IDetailCategoryBuilder& InfoCategory = DetailBuilder.EditCategory("Schedule", LOCTEXT("InfoCategory", "Basic Info"), ECategoryPriority::Important);
	IDetailCategoryBuilder& EntriesCategory = DetailBuilder.EditCategory("Schedule", LOCTEXT("EntriesCategory", "Schedule Entries"), ECategoryPriority::Important);
	IDetailCategoryBuilder& FallbackCategory = DetailBuilder.EditCategory("Schedule|Fallback", LOCTEXT("FallbackCategory", "Fallback Behavior"), ECategoryPriority::Default);
	IDetailCategoryBuilder& SettingsCategory = DetailBuilder.EditCategory("Schedule|Settings", LOCTEXT("SettingsCategory", "Settings"), ECategoryPriority::Default);

	// Add helpful description
	InfoCategory.AddCustomRow(LOCTEXT("DescriptionRow", "Description"))
		.WholeRowContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ScheduleDescription", "A complete schedule defining actor behavior throughout the day"))
			.AutoWrapText(true)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

#undef LOCTEXT_NAMESPACE
