// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/NeedsPanelWidget.h"
#include "Widgets/NeedDisplayWidget.h"
#include "Components/CharacterNeedsComponent.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Library/NeedsBlueprintLibrary.h"
#include "Data/NeedDefinitionAsset.h"
#include "Subsystems/NeedsSubsystem.h"

void UNeedsPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Set default widget class if not set
	if (!NeedWidgetClass)
	{
		NeedWidgetClass = UNeedDisplayWidget::StaticClass();
	}

	// Initialize
	if (TargetComponent)
	{
		CreateNeedWidgets();
		RegisterAsListener();
		RefreshDisplay();
	}
}

void UNeedsPanelWidget::NativeDestruct()
{
	UnregisterAsListener();
	Super::NativeDestruct();
}

// ========================================
// PUBLIC API
// ========================================

void UNeedsPanelWidget::SetTargetComponent(UCharacterNeedsComponent* InComponent)
{
	if (TargetComponent == InComponent)
	{
		return;
	}

	// Unregister from old component
	UnregisterAsListener();

	// Set new component
	TargetComponent = InComponent;

	// Clear and recreate widgets
	ClearNeedWidgets();

	if (TargetComponent)
	{
		CreateNeedWidgets();
		RegisterAsListener();
		RefreshDisplay();
	}
}

void UNeedsPanelWidget::RefreshDisplay()
{
	if (!TargetComponent)
	{
		return;
	}

	// Update all need widgets
	for (auto& Pair : NeedWidgets)
	{
		const FGameplayTag& NeedTag = Pair.Key;
		UNeedDisplayWidget* Widget = Pair.Value;

		if (Widget)
		{
			const float Value = TargetComponent->GetNeedValue_Implementation(NeedTag);
			const ENeedThresholdLevel Threshold = TargetComponent->GetNeedThreshold_Implementation(NeedTag);
			Widget->UpdateDisplay(Value, Threshold);
		}
	}

	// Update mood
	UpdateMoodDisplay();

	// Trigger Blueprint event
	BP_OnPanelRefreshed();
}

void UNeedsPanelWidget::ClearNeedWidgets()
{
	if (NeedsContainer)
	{
		NeedsContainer->ClearChildren();
	}

	NeedWidgets.Empty();
}

// ========================================
// INeedsListener INTERFACE
// ========================================

void UNeedsPanelWidget::OnNeedValueChanged_Implementation(const FNeedHandle& NeedHandle, float OldValue, float NewValue)
{
	if (!bAutoUpdate)
	{
		return;
	}

	TObjectPtr<UNeedDisplayWidget>* WidgetPtr = NeedWidgets.Find(NeedHandle.NeedTag);
	if (WidgetPtr && *WidgetPtr)
	{
		const ENeedThresholdLevel Threshold = TargetComponent->GetNeedThreshold_Implementation(NeedHandle.NeedTag);
		(*WidgetPtr)->UpdateDisplay(NewValue, Threshold);
	}
}

void UNeedsPanelWidget::OnNeedThresholdChanged_Implementation(const FNeedHandle& NeedHandle, ENeedThresholdLevel OldLevel, ENeedThresholdLevel NewLevel)
{
	if (!bAutoUpdate)
	{
		return;
	}

	TObjectPtr<UNeedDisplayWidget>* WidgetPtr = NeedWidgets.Find(NeedHandle.NeedTag);
	if (WidgetPtr && *WidgetPtr)
	{
		const float Value = TargetComponent->GetNeedValue_Implementation(NeedHandle.NeedTag);
		(*WidgetPtr)->UpdateDisplay(Value, NewLevel);
	}
}

void UNeedsPanelWidget::OnMoodChanged_Implementation(float OldMood, float NewMood)
{
	if (!bAutoUpdate)
	{
		return;
	}

	CurrentMood = NewMood;
	UpdateMoodDisplay();
	BP_OnMoodUpdated(NewMood);
}

void UNeedsPanelWidget::OnEffectApplied_Implementation(const FActiveNeedEffect& Effect)
{
	// Can be overridden in Blueprint to show effect notifications
}

void UNeedsPanelWidget::OnEffectExpired_Implementation(const FActiveNeedEffect& Effect)
{
	// Can be overridden in Blueprint to show effect expiration
}

void UNeedsPanelWidget::OnModifierAdded_Implementation(const FActiveNeedModifier& Modifier)
{
	// Can be overridden in Blueprint to show modifier notifications
}

void UNeedsPanelWidget::OnModifierRemoved_Implementation(const FActiveNeedModifier& Modifier)
{
	// Can be overridden in Blueprint to show modifier removal
}

// ========================================
// INTERNAL HELPERS
// ========================================

void UNeedsPanelWidget::CreateNeedWidgets()
{
	if (!TargetComponent || !NeedsContainer || !NeedWidgetClass)
	{
		return;
	}

	// Clear existing widgets
	ClearNeedWidgets();

	// Get needs from component
	const TArray<FGameplayTag> NeedTags = TargetComponent->GetAllNeedTags();

	// Get subsystem for need definitions
	UNeedsSubsystem* Subsystem = UNeedsBlueprintLibrary::GetNeedsSubsystem(this);
	if (!Subsystem)
	{
		return;
	}

	UNeedsSettings* Settings = Subsystem->GetSettings();
	if (!Settings)
	{
		return;
	}

	// Create widget for each need
	for (const FGameplayTag& NeedTag : NeedTags)
	{
		// Find definition
		UNeedDefinitionAsset* NeedDef = Settings->FindNeedDefinition(NeedTag);

		// Create widget
		UNeedDisplayWidget* NeedWidget = CreateWidget<UNeedDisplayWidget>(this, NeedWidgetClass);
		if (NeedWidget)
		{
			NeedWidget->NeedTag = NeedTag;
			NeedWidget->SetNeedDefinition(NeedDef);

			// Get initial values
			const float Value = TargetComponent->GetNeedValue_Implementation(NeedTag);
			const ENeedThresholdLevel Threshold = TargetComponent->GetNeedThreshold_Implementation(NeedTag);
			NeedWidget->UpdateDisplay(Value, Threshold);

			// Add to container
			NeedsContainer->AddChild(NeedWidget);

			// Store reference
			NeedWidgets.Add(NeedTag, NeedWidget);
		}
	}
}

void UNeedsPanelWidget::UpdateMoodDisplay()
{
	if (!TargetComponent)
	{
		return;
	}

	CurrentMood = TargetComponent->GetCurrentMood_Implementation();

	if (MoodTextBlock && bShowMood)
	{
		// Update text
		const FText MoodText = FText::Format(
			FText::FromString(TEXT("Mood: {0} ({1}))")),
			UNeedsBlueprintLibrary::MoodToText(CurrentMood),
			FText::AsNumber(FMath::RoundToInt(CurrentMood))
		);
		MoodTextBlock->SetText(MoodText);

		// Update color
		const FLinearColor MoodColor = UNeedsBlueprintLibrary::MoodToColor(CurrentMood);
		MoodTextBlock->SetColorAndOpacity(FSlateColor(MoodColor));

		MoodTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else if (MoodTextBlock)
	{
		MoodTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UNeedsPanelWidget::RegisterAsListener()
{
	if (TargetComponent)
	{
		TargetComponent->RegisterListener_Implementation(this);
	}
}

void UNeedsPanelWidget::UnregisterAsListener()
{
	if (TargetComponent)
	{
		TargetComponent->UnregisterListener_Implementation(this);
	}
}
