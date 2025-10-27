// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/NeedDisplayWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Library/NeedsBlueprintLibrary.h"

void UNeedDisplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Initialize visuals
	UpdateVisuals();
}

void UNeedDisplayWidget::UpdateDisplay(float NewValue, ENeedThresholdLevel NewThreshold)
{
	const float OldValue = CurrentValue;
	const ENeedThresholdLevel OldThreshold = CurrentThreshold;

	CurrentValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	CurrentThreshold = NewThreshold;

	// Update visuals
	UpdateVisuals();

	// Trigger Blueprint events
	if (!FMath::IsNearlyEqual(OldValue, CurrentValue, 0.01f))
	{
		BP_OnValueChanged(OldValue, CurrentValue);
	}

	if (OldThreshold != CurrentThreshold)
	{
		BP_OnThresholdChanged(OldThreshold, CurrentThreshold);
	}
}

void UNeedDisplayWidget::SetNeedDefinition(UNeedDefinitionAsset* InDefinition)
{
	NeedDefinition = InDefinition;
	
	if (NeedDefinition)
	{
		NeedTag = NeedDefinition->NeedTag;
	}

	UpdateVisuals();
}

FLinearColor UNeedDisplayWidget::GetThresholdColor_Implementation() const
{
	return UNeedsBlueprintLibrary::ThresholdToColor(CurrentThreshold);
}

// ========================================
// INTERNAL HELPERS
// ========================================

void UNeedDisplayWidget::UpdateVisuals()
{
	UpdateProgressBar();
	UpdateTextElements();
	UpdateIcon();
	UpdateThresholdIndicator();
}

void UNeedDisplayWidget::UpdateProgressBar()
{
	if (ValueProgressBar)
	{
		const float NormalizedValue = CurrentValue / 100.0f;
		ValueProgressBar->SetPercent(NormalizedValue);

		// Set color based on threshold
		const FLinearColor ThresholdColor = GetThresholdColor();
		ValueProgressBar->SetFillColorAndOpacity(ThresholdColor);
	}
}

void UNeedDisplayWidget::UpdateTextElements()
{
	// Update name
	if (NameTextBlock && bShowName)
	{
		if (NeedDefinition)
		{
			NameTextBlock->SetText(NeedDefinition->DisplayName);
		}
		else
		{
			NameTextBlock->SetText(FText::FromName(NeedTag.GetTagName()));
		}

		NameTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else if (NameTextBlock)
	{
		NameTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	// Update value
	if (ValueTextBlock && bShowValue)
	{
		const FText ValueText = FText::AsNumber(FMath::RoundToInt(CurrentValue));
		ValueTextBlock->SetText(ValueText);
		ValueTextBlock->SetVisibility(ESlateVisibility::Visible);

		// Color based on threshold
		const FLinearColor ThresholdColor = GetThresholdColor();
		ValueTextBlock->SetColorAndOpacity(FSlateColor(ThresholdColor));
	}
	else if (ValueTextBlock)
	{
		ValueTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNeedDisplayWidget::UpdateIcon()
{
	if (IconImage && bShowIcon)
	{
		if (NeedDefinition && NeedDefinition->Icon)
		{
			IconImage->SetBrushFromTexture(NeedDefinition->Icon);
			IconImage->SetVisibility(ESlateVisibility::Visible);

			// Tint icon based on threshold
			const FLinearColor ThresholdColor = GetThresholdColor();
			IconImage->SetColorAndOpacity(ThresholdColor);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else if (IconImage)
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNeedDisplayWidget::UpdateThresholdIndicator()
{
	if (ThresholdIndicator && bShowThreshold)
	{
		const FLinearColor ThresholdColor = GetThresholdColor();
		ThresholdIndicator->SetColorAndOpacity(ThresholdColor);

		// Show/hide based on threshold level
		if (CurrentThreshold == ENeedThresholdLevel::Normal)
		{
			ThresholdIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			ThresholdIndicator->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (ThresholdIndicator)
	{
		ThresholdIndicator->SetVisibility(ESlateVisibility::Collapsed);
	}
}
