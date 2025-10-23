// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/DialogueHistoryEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"

void UDialogueHistoryEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Play fade-in animation
    if (FadeInAnimation)
    {
        PlayAnimation(FadeInAnimation);
    }
}

void UDialogueHistoryEntryWidget::SetHistoryData(const FText& InSpeakerName, const FText& InDialogueText, UTexture2D* InPortrait, bool bInIsPlayerChoice)
{
    bIsPlayerChoice = bInIsPlayerChoice;

    // Set speaker name
    if (SpeakerName)
    {
   SpeakerName->SetText(InSpeakerName);
    }

    // Set dialogue text
    if (DialogueText)
    {
        DialogueText->SetText(InDialogueText);
    }

    // Set portrait
    if (SpeakerPortrait && InPortrait)
    {
        SpeakerPortrait->SetBrushFromTexture(InPortrait);
   SpeakerPortrait->SetVisibility(ESlateVisibility::Visible);
    }
    else if (SpeakerPortrait)
    {
    SpeakerPortrait->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Update styling based on speaker type
    UpdateStyling();

    // Call blueprint event
 OnHistoryEntryInitialized(bIsPlayerChoice);
}

void UDialogueHistoryEntryWidget::SetTimestamp(const FText& InTimestamp)
{
    if (Timestamp)
    {
        Timestamp->SetText(InTimestamp);
        Timestamp->SetVisibility(ESlateVisibility::Visible);
    }
}

void UDialogueHistoryEntryWidget::UpdateStyling()
{
    if (!EntryBorder)
    {
     return;
    }

    // Different styling for player vs NPC
    // This is a simple example - you can customize in Blueprint
    if (bIsPlayerChoice)
  {
        // Player choices: align right, different color
        // You can set this via Blueprint styles
    }
    else
    {
        // NPC dialogue: align left, default color
    }
}
