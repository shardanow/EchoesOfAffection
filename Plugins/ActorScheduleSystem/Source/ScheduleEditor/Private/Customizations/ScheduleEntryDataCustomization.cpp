// Copyright Epic Games, Inc. All Rights Reserved.

#include "Customizations/ScheduleEntryDataCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Data/ScheduleEntryData.h"

#define LOCTEXT_NAMESPACE "ScheduleEntryDataCustomization"

TSharedRef<IDetailCustomization> FScheduleEntryDataCustomization::MakeInstance()
{
	return MakeShareable(new FScheduleEntryDataCustomization);
}

void FScheduleEntryDataCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	CachedDetailBuilder = &DetailBuilder;

	// Reorganize categories for better workflow
	IDetailCategoryBuilder& InfoCategory = DetailBuilder.EditCategory("Schedule", LOCTEXT("InfoCategory", "Basic Info"), ECategoryPriority::Important);
	IDetailCategoryBuilder& TimingCategory = DetailBuilder.EditCategory("Schedule|Timing", LOCTEXT("TimingCategory", "Timing & Conditions"), ECategoryPriority::Important);
	IDetailCategoryBuilder& ActionCategory = DetailBuilder.EditCategory("Schedule|Action", LOCTEXT("ActionCategory", "Action"), ECategoryPriority::Important);
	IDetailCategoryBuilder& LocationCategory = DetailBuilder.EditCategory("Schedule|Location", LOCTEXT("LocationCategory", "Location"), ECategoryPriority::Default);
	IDetailCategoryBuilder& ExecutionCategory = DetailBuilder.EditCategory("Schedule|Execution", LOCTEXT("ExecutionCategory", "Execution Settings"), ECategoryPriority::Default);
	IDetailCategoryBuilder& ConditionsCategory = DetailBuilder.EditCategory("Schedule|Conditions", LOCTEXT("ConditionsCategory", "Additional Conditions"), ECategoryPriority::Default);

	// Add helpful description text
	InfoCategory.AddCustomRow(LOCTEXT("DescriptionRow", "Description"))
		.WholeRowContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("EntryDescription", "Configure when and what action this entry executes"))
			.AutoWrapText(true)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

#undef LOCTEXT_NAMESPACE
