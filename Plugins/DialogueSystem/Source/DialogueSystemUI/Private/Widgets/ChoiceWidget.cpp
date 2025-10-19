// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/ChoiceWidget.h"
#include "Core/DialogueNode.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UChoiceWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ChoiceButton)
    {
        ChoiceButton->OnClicked.AddDynamic(this, &UChoiceWidget::HandleButtonClicked);
    }

    UpdateVisuals();
}

void UChoiceWidget::InitializeChoice(UDialogueNode* Node, int32 Index)
{
    SetChoiceData(Node, Index);
}

void UChoiceWidget::SetChoiceData(UDialogueNode* Node, int32 Index)
{
    AssociatedNode = Node;
    ChoiceIndex = Index;

    UpdateVisuals();
    OnChoiceInitialized(Node, Index);
}

void UChoiceWidget::HandleButtonClicked()
{
    OnChoiceSelected.Broadcast(ChoiceIndex);
}

void UChoiceWidget::UpdateVisuals()
{
    if (!AssociatedNode)
    {
        return;
    }

    // Update choice text
    if (ChoiceText)
    {
        FText ChoiceTextContent = AssociatedNode->GetChoiceText();
        ChoiceText->SetText(ChoiceTextContent);
    }

    // Update hotkey display
    if (HotkeyText && ChoiceIndex >= 0)
    {
        FString HotkeyString = FString::Printf(TEXT("%d"), ChoiceIndex + 1);
        HotkeyText->SetText(FText::FromString(HotkeyString));
    }

    // Update consequence icon if available
    if (ConsequenceIcon && AssociatedNode->Tags.HasAnyExact(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Dialogue.Consequence")))))
    {
        ConsequenceIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else if (ConsequenceIcon)
    {
        ConsequenceIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}
