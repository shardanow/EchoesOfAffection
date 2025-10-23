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
	UE_LOG(LogTemp, Log, TEXT("UDialogueWidget::BindToRunner called"));

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

    if (UDialogueNode* CurrentNode = Runner->GetCurrentNode())
    {
        HandleNodeEntered(CurrentNode);
		HandleChoicesReady(Runner->GetAvailableChoices());    
    }

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
	UE_LOG(LogTemp, Log, TEXT("UDialogueWidget::HandleNodeEntered called"));

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
    TypewriterAccum = 0.0f;               
    bIsTyping = bEnableTypewriter && !FullText.IsEmpty();

    if (!bEnableTypewriter)
    {
        VisibleText = FullText;
        CurrentCharIndex = FullText.Len();
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

    if (CharactersPerSecond <= 0.f)
    {
        // защита: мгновенно показать весь текст
        VisibleText = FullText;
        CurrentCharIndex = FullText.Len();
        bIsTyping = false;
        if (DialogueText) { DialogueText->SetText(FText::FromString(VisibleText)); }
        return;
    }

    // накапливаем дробную часть, чтобы не тер€ть <1 символа за кадр
    TypewriterAccum += CharactersPerSecond * DeltaTime;

    const int32 Step = FMath::FloorToInt(TypewriterAccum);
    if (Step <= 0)
    {
        return; // ещЄ не набежал целый символ Ч ждЄм
    }

    TypewriterAccum -= Step; // оставл€ем хвост на следующий кадр

    const int32 NewIndex = FMath::Min(CurrentCharIndex + Step, FullText.Len());
    if (NewIndex == CurrentCharIndex)
    {
        return;
    }

    CurrentCharIndex = NewIndex;
    VisibleText = FullText.Left(CurrentCharIndex);

    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
    }

    if (CurrentCharIndex >= FullText.Len())
    {
        bIsTyping = false;
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
