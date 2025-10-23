// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/DialogueWidget.h"
#include "Core/DialogueRunner.h"
#include "Core/DialogueNode.h"
#include "Widgets/ChoiceWidget.h"
#include "Widgets/DialogueHistoryEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/ScrollBox.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Animation/WidgetAnimation.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetTree.h"

void UDialogueWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Initialize visual state
    if (ContinueIndicator)
    {
        ContinueIndicator->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (TypingIndicator)
    {
        TypingIndicator->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (SkipHintText)
    {
        SkipHintText->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Play intro animation
    if (bEnableAnimations && IntroAnimation)
    {
        PlayAnimation(IntroAnimation);
    }

    PlayIntroAnimation();

    // Play dialogue open sound
    if (DialogueOpenSound)
    {
      UGameplayStatics::PlaySound2D(this, DialogueOpenSound);
    }
}

void UDialogueWidget::NativeDestruct()
{
    UnbindFromRunner();

    // Play outro animation
    if (bEnableAnimations && OutroAnimation)
    {
PlayAnimation(OutroAnimation);
    }

    PlayOutroAnimation();

    // Play dialogue close sound
    if (DialogueCloseSound)
    {
        UGameplayStatics::PlaySound2D(this, DialogueCloseSound);
    }

    Super::NativeDestruct();
}

void UDialogueWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateTypewriter(InDeltaTime);
}

void UDialogueWidget::BindToRunner(UDialogueRunner* Runner)
{
    if (!Runner)
    {
        return;
    }

    UnbindFromRunner();

    CurrentRunner = Runner;

    // Subscribe to events
    CurrentRunner->OnNodeEntered.AddDynamic(this, &UDialogueWidget::HandleNodeEntered);
    CurrentRunner->OnChoicesReady.AddDynamic(this, &UDialogueWidget::HandleChoicesReady);
    CurrentRunner->OnDialogueEnded.AddDynamic(this, &UDialogueWidget::HandleDialogueEnded);
}

void UDialogueWidget::UnbindFromRunner()
{
    if (!CurrentRunner)
    {
        return;
    }

    CurrentRunner->OnNodeEntered.RemoveDynamic(this, &UDialogueWidget::HandleNodeEntered);
    CurrentRunner->OnChoicesReady.RemoveDynamic(this, &UDialogueWidget::HandleChoicesReady);
    CurrentRunner->OnDialogueEnded.RemoveDynamic(this, &UDialogueWidget::HandleDialogueEnded);

    CurrentRunner = nullptr;
}

void UDialogueWidget::SkipTypewriter()
{
    if (!bIsTyping)
  {
     return;
    }

  VisibleText = FullText;
    CurrentCharIndex = FullText.Len();
    bIsTyping = false;

    if (DialogueText)
    {
      DialogueText->SetText(FText::FromString(VisibleText));
    }

    UpdateTypingIndicator(false);
    
    // ✨ IMPROVED: Show choices NOW if they were waiting
    // Check if we have created but hidden choices
  if (bIsShowingChoices && ChoiceWidgets.Num() > 0 && ChoicesContainer)
    {
  // Typewriter skipped - now show the choices!
     ChoicesContainer->SetVisibility(ESlateVisibility::Visible);
      UE_LOG(LogTemp, Log, TEXT("DialogueWidget::SkipTypewriter - Showing choices after skip"));
        
  // Animate choices appearance
 if (bEnableAnimations)
        {
     AnimateChoicesAppear();
        }
     
   // Don't show continue indicator when we have choices
    }
    else
    {
     // No choices - show continue indicator
  UpdateContinueIndicator(true);
    }

    OnTypewriterCompleted();

    if (SkipHintText)
    {
        SkipHintText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDialogueWidget::SelectChoice(int32 Index)
{
    if (!CurrentRunner)
    {
 return;
    }

 // Get the selected choice node before navigating
    if (Index >= 0 && Index < ChoiceWidgets.Num())
    {
     UChoiceWidget* SelectedWidget = ChoiceWidgets[Index];
        if (SelectedWidget)
        {
  UDialogueNode* ChoiceNode = SelectedWidget->GetNode();
if (ChoiceNode && bShowHistory)
            {
  // Add player choice to history
        FText PlayerName = FText::FromString(TEXT("You")); // TODO: Get actual player name
     FText ChoiceText = ChoiceNode->GetChoiceText();
   
         AddToHistory(PlayerName, ChoiceText, nullptr);
                
  UE_LOG(LogTemp, Log, TEXT("DialogueWidget::SelectChoice - Added player choice to history: %s"), 
         *ChoiceText.ToString());
   }
        }
    }

    // Play choice select sound
    if (ChoiceSelectSound)
    {
        UGameplayStatics::PlaySound2D(this, ChoiceSelectSound);
    }

    CurrentRunner->SelectChoice(Index);

    // Clear choices
    ClearChoiceButtons();
    
    // DON'T reset bWaitingForInput here!
    // The next node will set it appropriately in HandleNodeEntered
    // bWaitingForInput is managed by the node flow, not by choice selection
    
    UpdateContinueIndicator(false);
}

void UDialogueWidget::ContinueDialogue()
{
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::ContinueDialogue called - bWaitingForInput=%d, CurrentRunner=%s, bIsTyping=%d"), 
        bWaitingForInput, CurrentRunner ? TEXT("Valid") : TEXT("NULL"), bIsTyping);

    if (!bWaitingForInput || !CurrentRunner)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::ContinueDialogue - Early return! bWaitingForInput=%d, CurrentRunner=%s"), 
        bWaitingForInput, CurrentRunner ? TEXT("Valid") : TEXT("NULL"));
        return;
    }

    if (bIsTyping)
    {
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::ContinueDialogue - Skipping typewriter"));
        SkipTypewriter();
      return;
    }

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::ContinueDialogue - Calling Runner->Continue()"));
    CurrentRunner->Continue();
    
    // DON'T reset bWaitingForInput here!
    // The next node will set it appropriately in HandleNodeEntered
    // bWaitingForInput will be managed by the node flow:
    // - HandleNodeEntered sets it to true for NPC/End nodes
    // - SelectChoice sets it to false when navigating after choice
  // - HandleDialogueEnded sets it to false when dialogue ends
    
    UpdateContinueIndicator(false);
}

void UDialogueWidget::ClearHistory()
{
    if (HistoryScrollBox)
    {
      HistoryScrollBox->ClearChildren();
    }
}

void UDialogueWidget::HandleNodeEntered(UDialogueNode* Node)
{
    if (!Node)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::HandleNodeEntered - Node is null!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleNodeEntered - Node: %s"), *Node->NodeId.ToString());

    // Clear the showing choices flag now that we're entering a new node
    bIsShowingChoices = false;

    CurrentNode = Node;
    // DON'T reset bWaitingForInput here - it will be set correctly later
    // bWaitingForInput = false;  ← REMOVED! This was causing the bug!

    // Clear previous choices
    ClearChoiceButtons();
    
    // Hide choices container when showing dialogue text
    if (ChoicesContainer)
    {
      ChoicesContainer->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Play new node animation
    if (bEnableAnimations && NewNodeAnimation)
    {
   PlayAnimation(NewNodeAnimation);
    }

    PlayNewNodeAnimation();
    OnNodeChanged(Node);

    // Update visuals
    const UDialogueSessionContext* Context = CurrentRunner ? CurrentRunner->GetContext() : nullptr;

  UpdatePortrait(Node, Context);
    UpdateSpeakerName(Node);

    // Update emotion
    FGameplayTag EmotionTag = Node->GetEmotionTag();
    if (EmotionTag.IsValid())
    {
        UpdateEmotion(EmotionTag);
    }

    // Start typewriter
    FText DialogueTextContent = Node->GetDialogueText();
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleNodeEntered - Dialogue Text: %s"), *DialogueTextContent.ToString());
    
    StartTypewriter(DialogueTextContent);

    // Add to history
    if (bShowHistory)
    {
        AddToHistory(Node->GetSpeakerName(), DialogueTextContent, nullptr);
    }

    // Show typing indicator
    UpdateTypingIndicator(bIsTyping);

    // Show skip hint
    if (SkipHintText && bIsTyping)
    {
        SkipHintText->SetVisibility(ESlateVisibility::Visible);
    }
 
    // ✨ FIXED: ALWAYS enable waiting for input to allow skipping typewriter
    // This works for both nodes with and without choices
    // Choices will override continue indicator later, but input is still enabled
    if (bAlwaysShowContinueIndicator &&
        (Node->NodeType == EDialogueNodeType::End || 
       Node->NodeType == EDialogueNodeType::NpcLine))
  {
        SetWaitingForInput(true, TEXT("HandleNodeEntered - NPC/End node"));
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleNodeEntered - bWaitingForInput set to TRUE for node type: %d"), (int32)Node->NodeType);
     
     // Continue indicator will be shown after typewriter completes
        // or immediately if configured (but NOT during typewriter for choice nodes)
  if (!bIsTyping || bShowContinueIndicatorDuringTypewriter)
        {
     UpdateContinueIndicator(true);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::HandleNodeEntered - bWaitingForInput NOT set! bAlwaysShowContinueIndicator=%d, NodeType=%d"), 
       bAlwaysShowContinueIndicator, (int32)Node->NodeType);
    }
}

void UDialogueWidget::HandleChoicesReady(const TArray<UDialogueNode*>& Choices)
{
    if (!CurrentRunner)
    {
  return;
    }

UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleChoicesReady - START: bWaitingForInput=%d, bIsTyping=%d"), 
        bWaitingForInput, bIsTyping);

  // bWaitingForInput should already be true from HandleNodeEntered
    // ALWAYS set it to true for nodes with choices
    SetWaitingForInput(true, TEXT("HandleChoicesReady - choices available"));
UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleChoicesReady - AFTER SET: bWaitingForInput=%d"), bWaitingForInput);
    
    bIsShowingChoices = true; // ⚠️ Set flag to prevent clearing

    const UDialogueSessionContext* Context = CurrentRunner->GetContext();
    CreateChoiceButtons(Choices, Context);

    // ✨ IMPROVED UX: Don't show choices until typewriter completes
    // This gives player time to read the dialogue before making a choice
    if (bIsTyping)
    {
 // Choices are created but hidden - they will be shown when typewriter completes
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleChoicesReady - Choices created but hidden until typewriter completes"));
        return;
    }

    // Show choices container immediately if typewriter is not active
    if (ChoicesContainer)
    {
        ChoicesContainer->SetVisibility(ESlateVisibility::Visible);
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::HandleChoicesReady - Set ChoicesContainer to Visible"));
    }

    // Hide continue indicator when showing choices
    UpdateContinueIndicator(false);

    // Animate choices
    if (bEnableAnimations)
  {
        AnimateChoicesAppear();
    }
}

void UDialogueWidget::HandleDialogueEnded()
{
    CurrentNode = nullptr;
    SetWaitingForInput(false, TEXT("HandleDialogueEnded"));
    bIsTyping = false;

    ClearChoiceButtons();
    UpdateContinueIndicator(false);
    UpdateTypingIndicator(false);

    // Play outro animation
    if (bEnableAnimations && OutroAnimation)
    {
        PlayAnimation(OutroAnimation);
}

    PlayOutroAnimation();
}

void UDialogueWidget::UpdatePortrait(const UDialogueNode* Node, const UDialogueSessionContext* Context)
{
    if (!SpeakerPortrait || !bShowPortrait || !Node)
    {
        return;
    }

    UTexture2D* Portrait = Node->GetSpeakerPortrait();
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

void UDialogueWidget::UpdateEmotion(const FGameplayTag& EmotionTag)
{
    if (!EmotionIcon || !bShowEmotions || !EmotionTag.IsValid())
    {
        return;
    }

    // Blueprint event for custom emotion handling
    OnEmotionChanged(EmotionTag);

    // Show emotion icon
    EmotionIcon->SetVisibility(ESlateVisibility::Visible);
}

void UDialogueWidget::UpdateContinueIndicator(bool bShow)
{
    if (!ContinueIndicator)
    {
   return;
  }

    if (bShow)
    {
 ContinueIndicator->SetVisibility(ESlateVisibility::Visible);

      // Play blink animation
        if (bEnableAnimations && ContinueBlinkAnimation)
        {
            PlayAnimation(ContinueBlinkAnimation, 0.0f, 0); // Loop indefinitely
   }
    }
    else
    {
  ContinueIndicator->SetVisibility(ESlateVisibility::Collapsed);

      // Stop blink animation
        if (ContinueBlinkAnimation)
        {
     StopAnimation(ContinueBlinkAnimation);
   }
    }
}

void UDialogueWidget::UpdateTypingIndicator(bool bShow)
{
    if (!TypingIndicator)
    {
     return;
    }

    if (bShow)
  {
        TypingIndicator->SetVisibility(ESlateVisibility::Visible);

      // Play typing animation
 if (bEnableAnimations && TypingAnimation)
    {
            PlayAnimation(TypingAnimation, 0.0f, 0); // Loop indefinitely
        }
    }
    else
    {
        TypingIndicator->SetVisibility(ESlateVisibility::Collapsed);

// Stop typing animation
        if (TypingAnimation)
   {
            StopAnimation(TypingAnimation);
        }
    }
}

void UDialogueWidget::StartTypewriter(const FText& Text)
{
    FullText = Text.ToString();
    VisibleText = TEXT("");
    CurrentCharIndex = 0;
    TypewriterAccum = 0.0f;
    TypewriterSoundAccum = 0.0f;
    bIsTyping = bEnableTypewriter && !FullText.IsEmpty();

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::StartTypewriter - FullText: '%s', bEnableTypewriter: %d, bIsTyping: %d"), 
        *FullText, bEnableTypewriter, bIsTyping);

  if (!bEnableTypewriter || FullText.IsEmpty())
 {
        VisibleText = FullText;
     CurrentCharIndex = FullText.Len();
   bIsTyping = false;

   // Show continue indicator immediately
      UpdateContinueIndicator(true);
        
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::StartTypewriter - Typewriter disabled or empty text, showing full text immediately"));
    }

    if (DialogueText)
    {
        DialogueText->SetText(FText::FromString(VisibleText));
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::StartTypewriter - Set DialogueText to: '%s'"), *VisibleText);
    }
    else
    {
     UE_LOG(LogTemp, Error, TEXT("DialogueWidget::StartTypewriter - DialogueText widget is NULL!"));
    }

    UpdateTypingIndicator(bIsTyping);
}

void UDialogueWidget::UpdateTypewriter(float DeltaTime)
{
    if (!bIsTyping || FullText.IsEmpty())
  {
        return;
    }

    if (CharactersPerSecond <= 0.f)
    {
 SkipTypewriter();
     return;
    }

  // Accumulate time for character display
    TypewriterAccum += CharactersPerSecond * DeltaTime;
    TypewriterSoundAccum += DeltaTime;

 const int32 Step = FMath::FloorToInt(TypewriterAccum);
    if (Step <= 0)
    {
        return;
    }

    TypewriterAccum -= Step;

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

    // Play typewriter sound
    if (bEnableTypewriterSound && TypewriterSoundAccum >= TypewriterSoundInterval)
    {
  PlayTypewriterSound();
        TypewriterSoundAccum = 0.0f;
    }

    // Check if completed
    if (CurrentCharIndex >= FullText.Len())
    {
    bIsTyping = false;
        UpdateTypingIndicator(false);
        
        // ✨ IMPROVED: Show choices NOW if they were waiting
  // Check if we have created but hidden choices
        if (bIsShowingChoices && ChoiceWidgets.Num() > 0 && ChoicesContainer)
        {
            // Typewriter completed - now show the choices!
 ChoicesContainer->SetVisibility(ESlateVisibility::Visible);
            UE_LOG(LogTemp, Log, TEXT("DialogueWidget::UpdateTypewriter - Typewriter completed, showing choices"));
          
      // Animate choices appearance
if (bEnableAnimations)
 {
       AnimateChoicesAppear();
     }
    
  // Don't show continue indicator when we have choices
    }
  else
        {
 // No choices - show continue indicator
 UpdateContinueIndicator(true);
   }
        
    OnTypewriterCompleted();

        if (SkipHintText)
        {
      SkipHintText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UDialogueWidget::PlayTypewriterSound()
{
    if (TypewriterSound)
 {
        UGameplayStatics::PlaySound2D(this, TypewriterSound);
    }
}

void UDialogueWidget::CreateChoiceButtons(const TArray<UDialogueNode*>& Choices, const UDialogueSessionContext* Context)
{
    ClearChoiceButtons();

    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::CreateChoiceButtons - Creating %d choices"), Choices.Num());

    if (!ChoicesContainer)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget::CreateChoiceButtons - ChoicesContainer is NULL!"));
     return;
    }
    
    if (!ChoiceWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget::CreateChoiceButtons - ChoiceWidgetClass is NULL!"));
        return;
    }

    for (int32 i = 0; i < Choices.Num(); ++i)
    {
        UDialogueNode* ChoiceNode = Choices[i];
        if (!ChoiceNode)
    {
            UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::CreateChoiceButtons - Choice %d is null"), i);
            continue;
   }

     UE_LOG(LogTemp, Log, TEXT("DialogueWidget::CreateChoiceButtons - Creating choice %d: %s"), 
  i, *ChoiceNode->GetChoiceText().ToString());

        UChoiceWidget* ChoiceWidget = CreateWidget<UChoiceWidget>(this, ChoiceWidgetClass);
   if (ChoiceWidget)
        {
   ChoiceWidget->SetChoiceData(ChoiceNode, i);
            ChoiceWidget->OnChoiceSelected.AddDynamic(this, &UDialogueWidget::SelectChoice);

   // Set hover sound
        if (ChoiceHoverSound)
            {
   ChoiceWidget->SetHoverSound(ChoiceHoverSound);
            }

     ChoicesContainer->AddChild(ChoiceWidget);
       ChoiceWidgets.Add(ChoiceWidget);
      
      UE_LOG(LogTemp, Log, TEXT("DialogueWidget::CreateChoiceButtons - Successfully created and added choice %d"), i);
        }
    else
        {
 UE_LOG(LogTemp, Error, TEXT("DialogueWidget::CreateChoiceButtons - Failed to create choice widget %d"), i);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::CreateChoiceButtons - Total widgets created: %d"), ChoiceWidgets.Num());
}

void UDialogueWidget::ClearChoiceButtons()
{
    // Don't clear if we're currently showing choices
    if (bIsShowingChoices)
    {
        UE_LOG(LogTemp, Warning, TEXT("DialogueWidget::ClearChoiceButtons - Blocked: currently showing choices"));
   return;
 }

  if (ChoicesContainer)
    {
 ChoicesContainer->ClearChildren();
  // DON'T hide here - let HandleChoicesReady show it
  // Hide container when clearing choices
// ChoicesContainer->SetVisibility(ESlateVisibility::Collapsed);
    UE_LOG(LogTemp, Log, TEXT("DialogueWidget::ClearChoiceButtons - Cleared children (visibility unchanged)"));
    }

 ChoiceWidgets.Empty();
}

void UDialogueWidget::AnimateChoicesAppear()
{
    if (!bEnableAnimations || !ChoiceAppearAnimation)
    {
        return;
    }

    // Animate each choice with slight delay
    for (int32 i = 0; i < ChoiceWidgets.Num(); ++i)
    {
        UChoiceWidget* ChoiceWidget = ChoiceWidgets[i];
        if (ChoiceWidget)
        {
            float Delay = i * 0.1f; // 100ms delay between each choice
            ChoiceWidget->PlayAppearAnimation(Delay);
        }
    }
}

void UDialogueWidget::AddToHistory(const FText& InSpeakerName, const FText& InDialogueText, UTexture2D* Portrait)
{
    if (!HistoryScrollBox || !HistoryEntryWidgetClass)
    {
        return;
    }

    // Check history limit
    if (HistoryScrollBox->GetChildrenCount() >= MaxHistoryEntries)
    {
        // Remove oldest entry
        if (HistoryScrollBox->GetChildrenCount() > 0)
        {
   HistoryScrollBox->RemoveChildAt(0);
        }
    }

    // Create history entry - cast to specific type
    UDialogueHistoryEntryWidget* HistoryEntry = CreateWidget<UDialogueHistoryEntryWidget>(this, HistoryEntryWidgetClass);
    if (HistoryEntry)
    {
        // Set history entry data
        bool bIsPlayerChoice = false; // TODO: Determine if this is a player choice
        HistoryEntry->SetHistoryData(InSpeakerName, InDialogueText, Portrait, bIsPlayerChoice);
        
        HistoryScrollBox->AddChild(HistoryEntry);

        // Auto-scroll to bottom
        ScrollHistoryToBottom();
        
        UE_LOG(LogTemp, Log, TEXT("DialogueWidget::AddToHistory - Added entry: %s - %s"), 
            *InSpeakerName.ToString(), *InDialogueText.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget::AddToHistory - Failed to create history entry widget"));
    }
}

void UDialogueWidget::ScrollHistoryToBottom()
{
    if (HistoryScrollBox)
    {
        HistoryScrollBox->ScrollToEnd();
    }
}
