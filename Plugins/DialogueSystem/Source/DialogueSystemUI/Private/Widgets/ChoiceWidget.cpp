// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/ChoiceWidget.h"
#include "Core/DialogueNode.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Animation/WidgetAnimation.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UChoiceWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ChoiceButton)
    {
        ChoiceButton->OnClicked.AddDynamic(this, &UChoiceWidget::HandleButtonClicked);
        ChoiceButton->OnHovered.AddDynamic(this, &UChoiceWidget::HandleButtonHovered);
        ChoiceButton->OnUnhovered.AddDynamic(this, &UChoiceWidget::HandleButtonUnhovered);
    }

    // Initialize hover overlay
    if (HoverOverlay)
    {
        HoverOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    // DON'T call UpdateVisuals here - data not set yet!
    // It will be called in SetChoiceData after OnChoiceInitialized
    // UpdateVisuals();
}

void UChoiceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply UChoiceWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    // Handle hotkey input (1-9 keys)
    if (bShowHotkeys && ChoiceIndex >= 0 && ChoiceIndex < 9)
    {
        FKey PressedKey = InKeyEvent.GetKey();
        
     // Check if the pressed key matches this choice's hotkey
        FString ExpectedKeyName = FString::Printf(TEXT("%d"), ChoiceIndex + 1);
        if (PressedKey.ToString() == ExpectedKeyName || 
      PressedKey.ToString() == FString::Printf(TEXT("NumPadOne")) + ExpectedKeyName)
        {
       if (bIsAvailable)
            {
 HandleButtonClicked();
        return FReply::Handled();
    }
     }
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UChoiceWidget::InitializeChoice(UDialogueNode* Node, int32 Index)
{
    SetChoiceData(Node, Index);
}

void UChoiceWidget::SetChoiceData(UDialogueNode* Node, int32 Index)
{
    UE_LOG(LogTemp, Log, TEXT("ChoiceWidget::SetChoiceData - Node: %s, Index: %d"), 
        Node ? *Node->NodeId.ToString() : TEXT("NULL"), Index);

    AssociatedNode = Node;
    ChoiceIndex = Index;

    UpdateAvailability();
    
    // Call Blueprint event first - it may want to customize before we update visuals
    OnChoiceInitialized(Node, Index);
    
    // Always update visuals AFTER Blueprint event
    // Blueprint can override UpdateVisuals if needed
    UpdateVisuals();
}

void UChoiceWidget::SetHoverSound(USoundBase* Sound)
{
    HoverSound = Sound;
}

void UChoiceWidget::PlayAppearAnimation(float Delay)
{
    if (!bEnableAnimations || !AppearAnimation)
    {
    return;
    }

    if (Delay > 0.0f)
    {
        // Play with delay
        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
 if (AppearAnimation)
 {
   PlayAnimation(AppearAnimation);
    }
  }, Delay, false);
    }
    else
 {
   PlayAnimation(AppearAnimation);
    }
}

void UChoiceWidget::HandleButtonClicked()
{
    if (!bIsAvailable)
    {
        return;
    }

    // Play click animation
  if (bEnableAnimations && ClickAnimation)
    {
    PlayAnimation(ClickAnimation);
    }

    OnChoiceClicked();
    OnChoiceSelected.Broadcast(ChoiceIndex);
}

void UChoiceWidget::HandleButtonHovered()
{
    if (!bIsAvailable)
    {
        return;
    }

  bIsHovered = true;

    // Show hover overlay
  if (HoverOverlay)
    {
        HoverOverlay->SetVisibility(ESlateVisibility::Visible);
  }

    // Play hover animation
    if (bEnableHoverEffects && bEnableAnimations && HoverAnimation)
    {
        PlayAnimation(HoverAnimation);
    }

    // Play hover sound
    PlayHoverSound();

    OnChoiceHovered();
}

void UChoiceWidget::HandleButtonUnhovered()
{
    bIsHovered = false;

    // Hide hover overlay
    if (HoverOverlay)
 {
        HoverOverlay->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Play unhover animation
    if (bEnableHoverEffects && bEnableAnimations && UnhoverAnimation)
    {
        PlayAnimation(UnhoverAnimation);
    }

    OnChoiceUnhovered();
}

void UChoiceWidget::UpdateVisuals()
{
    if (!AssociatedNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChoiceWidget::UpdateVisuals - AssociatedNode is NULL"));
        return;
    }

    // Update choice text
    if (ChoiceText)
    {
    FText ChoiceTextContent = AssociatedNode->GetChoiceText();
        UE_LOG(LogTemp, Log, TEXT("ChoiceWidget::UpdateVisuals - Setting choice text: '%s'"), *ChoiceTextContent.ToString());
        ChoiceText->SetText(ChoiceTextContent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ChoiceWidget::UpdateVisuals - ChoiceText widget is NULL!"));
    }

    // Update hotkey display
    if (HotkeyText && bShowHotkeys && ChoiceIndex >= 0)
    {
        FString HotkeyString = FString::Printf(TEXT("%d"), ChoiceIndex + 1);
        HotkeyText->SetText(FText::FromString(HotkeyString));
        HotkeyText->SetVisibility(ESlateVisibility::Visible);
    }
    else if (HotkeyText)
    {
        HotkeyText->SetVisibility(ESlateVisibility::Collapsed);
    }

    UpdateStateIcons();

    // Update button state
    if (ChoiceButton)
    {
        ChoiceButton->SetIsEnabled(bIsAvailable);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ChoiceWidget::UpdateVisuals - ChoiceButton is NULL!"));
    }

    // Play disabled animation if not available
    if (!bIsAvailable && bEnableAnimations && DisabledAnimation)
    {
        PlayAnimation(DisabledAnimation);
    }
}

void UChoiceWidget::UpdateAvailability()
{
    if (!AssociatedNode)
    {
        bIsAvailable = false;
        return;
    }

    // Check if choice has requirements
  // This is a placeholder - implement your own logic
    bIsAvailable = true;

 // You can add checks here like:
    // - Check if player has required items
// - Check if player has required stats
    // - Check if choice was already selected before
    // - Check gameplay tags
}

void UChoiceWidget::UpdateStateIcons()
{
    if (!AssociatedNode)
    {
        return;
    }

    // Update consequence icon
    if (ConsequenceIcon)
    {
        bool bHasConsequence = AssociatedNode->Tags.HasAnyExact(
            FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Dialogue.Consequence")))
        );
  
        ConsequenceIcon->SetVisibility(bHasConsequence ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    // Update locked icon
    if (LockedIcon)
    {
        LockedIcon->SetVisibility(!bIsAvailable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

    // Update warning icon
    if (WarningIcon)
    {
        bool bHasWarning = AssociatedNode->Tags.HasAnyExact(
          FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Dialogue.Warning")))
        );
 
        WarningIcon->SetVisibility(bHasWarning ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UChoiceWidget::PlayHoverSound()
{
    if (HoverSound)
    {
        UGameplayStatics::PlaySound2D(this, HoverSound);
    }
}
