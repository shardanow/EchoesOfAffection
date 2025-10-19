// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/DialogueWidget.h"
#include "Widgets/ChoiceWidget.h"
#include "Core/DialogueRunner.h"
#include "Core/DialogueNode.h"
#include "Core/DialogueContext.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/RichTextBlock.h"

void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDialogueWidget::NativeDestruct()
{
    UnbindFromRunner();
    Super::NativeDestruct();
}

void UDialogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsTyping && bEnableTypewriter)
    {
        UpdateTypewriter(InDeltaTime);
    }
}

void UDialogueWidget::BindToRunner(UDialogueRunner* Runner)
{
    if (!Runner)
    {
        return;
    }

    UnbindFromRunner();

    CurrentRunner = Runner;

    // Bind to events
    Runner->OnNodeEntered.AddDynamic(this, &UDialogueWidget::HandleNodeEntered);
    Runner->OnChoicesReady.AddDynamic(this, &UDialogueWidget::HandleChoicesReady);
    Runner->OnDialogueEnded.AddDynamic(this, &UDialogueWidget::HandleDialogueEnded);

    PlayIntroAnimation();
}

void UDialogueWidget::UnbindFromRunner()
{
    if (CurrentRunner)
    {
        CurrentRunner->OnNodeEntered.RemoveDynamic(this, &UDialogueWidget::HandleNodeEntered);
        CurrentRunner->OnChoicesReady.RemoveDynamic(this, &UDialogueWidget::HandleChoicesReady);
        CurrentRunner->OnDialogueEnded.RemoveDynamic(this, &UDialogueWidget::HandleDialogueEnded);

        CurrentRunner = nullptr;
    }

    ClearChoiceButtons();
}

void UDialogueWidget::SkipTypewriter()
{
    if (bIsTyping)
    {
        bIsTyping = false;
        VisibleText = FullText;

        if (DialogueText)
        {
            DialogueText->SetText(FText::FromString(VisibleText));
        }
    }
}

void UDialogueWidget::SelectChoice(int32 Index)
{
    if (CurrentRunner && ChoiceWidgets.IsValidIndex(Index))
    {
        CurrentRunner->SelectChoice(Index);
    }
}

void UDialogueWidget::HandleNodeEntered(UDialogueNode* Node)
{
    if (!Node)
    {
        return;
    }

    UpdateSpeakerName(Node);
    UpdatePortrait(Node, CurrentRunner ? CurrentRunner->GetContext() : nullptr);

    FText DialogueTextContent = Node->GetDialogueText();
    StartTypewriter(DialogueTextContent);

    // Update emotion
    if (bShowEmotions && Node->EmotionTag.IsValid())
    {
        UpdateEmotion(Node->EmotionTag);
    }

    // Clear choices since we're at a new node
    ClearChoiceButtons();
}

void UDialogueWidget::HandleChoicesReady(const TArray<UDialogueNode*>& Choices)
{
    CreateChoiceButtons(Choices, CurrentRunner ? CurrentRunner->GetContext() : nullptr);
}

void UDialogueWidget::HandleDialogueEnded()
{
    PlayOutroAnimation();
    ClearChoiceButtons();
}

void UDialogueWidget::UpdatePortrait(const UDialogueNode* Node, const UDialogueSessionContext* Context)
{
    if (!bShowPortrait || !SpeakerPortrait)
    {
        return;
    }

    // Get portrait from node or context
    UTexture2D* Portrait = nullptr;

    if (Node && Node->SpeakerPortrait)
    {
        Portrait = Node->SpeakerPortrait.LoadSynchronous();
    }

    if (Portrait)
    {
        SpeakerPortrait->SetBrushFromTexture(Portrait);
        SpeakerPortrait->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SpeakerPortrait->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDialogueWidget::UpdateSpeakerName(const UDialogueNode* Node)
{
    if (!SpeakerName || !Node)
    {
        return;
    }

    FText Name = Node->GetSpeakerName();
    SpeakerName->SetText(Name);
}

void UDialogueWidget::StartTypewriter(const FText& Text)
{
    FullText = Text.ToString();
    VisibleText = TEXT("");
    CurrentCharIndex = 0;
    bIsTyping = bEnableTypewriter && !FullText.IsEmpty();

    if (!bEnableTypewriter)
    {
        VisibleText = FullText;
    }

    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
    }
}

void UDialogueWidget::UpdateTypewriter(float DeltaTime)
{
    if (!bIsTyping || FullText.IsEmpty())
    {
        return;
    }

    // Get typewriter speed from subsystem or use default
    float CharactersPerSecond = 30.0f; // Default speed

    float CharsToAdd = CharactersPerSecond * DeltaTime;
    CurrentCharIndex += FMath::FloorToInt(CharsToAdd);

    if (CurrentCharIndex >= FullText.Len())
    {
        CurrentCharIndex = FullText.Len();
        bIsTyping = false;
    }

    VisibleText = FullText.Left(CurrentCharIndex);

    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
    }
}

void UDialogueWidget::CreateChoiceButtons(const TArray<UDialogueNode*>& Choices, const UDialogueSessionContext* Context)
{
    ClearChoiceButtons();

    if (!ChoicesContainer || !ChoiceWidgetClass)
    {
        return;
    }

    for (int32 i = 0; i < Choices.Num(); ++i)
    {
        UDialogueNode* ChoiceNode = Choices[i];
        if (!ChoiceNode)
        {
            continue;
        }

        UChoiceWidget* ChoiceWidget = CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
        if (ChoiceWidget)
        {
            ChoiceWidget->SetChoiceData(ChoiceNode, i);
            ChoiceWidget->OnChoiceSelected.AddDynamic(this, &UDialogueWidget::SelectChoice);

            ChoicesContainer->AddChild(ChoiceWidget);
            ChoiceWidgets.Add(ChoiceWidget);
        }
    }
}

void UDialogueWidget::ClearChoiceButtons()
{
    if (ChoicesContainer)
    {
        ChoicesContainer->ClearChildren();
    }

    ChoiceWidgets.Empty();
}

void UDialogueWidget::UpdateEmotion(const FGameplayTag& EmotionTag)
{
    if (!EmotionIcon)
    {
        return;
    }

    // Here you would map emotion tags to emotion icons
    // For now, just show/hide the icon
    if (EmotionTag.IsValid())
    {
        EmotionIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        EmotionIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}
