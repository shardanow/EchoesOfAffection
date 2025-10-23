// Copyright Epic Games, Inc. All Rights Reserved.

#include "Widgets/DialogueEmotionWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Materials/MaterialInstanceDynamic.h"

void UDialogueEmotionWidget::NativeConstruct()
{
Super::NativeConstruct();

  // Create dynamic material instance if needed
    if (EmotionIcon && EmotionIcon->GetDynamicMaterial())
    {
        EmotionMaterial = EmotionIcon->GetDynamicMaterial();
    }

    // Hide by default
    SetVisibility(ESlateVisibility::Collapsed);
}

void UDialogueEmotionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bIsTransitioning)
    {
        UpdateTransition(InDeltaTime);
    }
}

void UDialogueEmotionWidget::SetEmotion(const FGameplayTag& EmotionTag, bool bAnimate)
{
    if (!EmotionTag.IsValid())
    {
        ClearEmotion(bAnimate);
        return;
    }

    // Store previous emotion
    PreviousEmotion = CurrentEmotion;
    CurrentEmotion = EmotionTag;

    // Make visible
    if (GetVisibility() == ESlateVisibility::Collapsed)
    {
        SetVisibility(ESlateVisibility::Visible);
        
        if (bAnimate && bEnableAnimations && AppearAnimation)
        {
            PlayAnimation(AppearAnimation);
   }
    }

    // Start transition
    if (bAnimate && bEnableAnimations)
    {
  bIsTransitioning = true;
        TransitionProgress = 0.0f;
        
    if (EmotionChangeAnimation)
   {
       PlayAnimation(EmotionChangeAnimation);
        }

   OnTransitionStarted(PreviousEmotion, CurrentEmotion);
    }
    else
    {
        ApplyEmotionVisuals(EmotionTag);
    }

    // Start pulse animation
    if (bEnableAnimations && PulseAnimation)
    {
 PlayAnimation(PulseAnimation, 0.0f, 0); // Loop indefinitely
 }

    OnEmotionChanged(CurrentEmotion, PreviousEmotion);
}

void UDialogueEmotionWidget::ClearEmotion(bool bAnimate)
{
    PreviousEmotion = CurrentEmotion;
    CurrentEmotion = FGameplayTag();

    if (bAnimate && bEnableAnimations && DisappearAnimation)
    {
        PlayAnimation(DisappearAnimation);
        
    // Hide after animation completes
        FTimerHandle TimerHandle;
        float AnimDuration = DisappearAnimation->GetEndTime();
     GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            SetVisibility(ESlateVisibility::Collapsed);
     }, AnimDuration, false);
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
    }

    // Stop pulse animation
    if (PulseAnimation)
    {
        StopAnimation(PulseAnimation);
    }

    OnEmotionCleared();
}

void UDialogueEmotionWidget::UpdateTransition(float DeltaTime)
{
    if (!bIsTransitioning || TransitionDuration <= 0.0f)
    {
        bIsTransitioning = false;
        ApplyEmotionVisuals(CurrentEmotion);
        OnTransitionCompleted();
   return;
    }

    TransitionProgress += DeltaTime / TransitionDuration;

    if (TransitionProgress >= 1.0f)
    {
        TransitionProgress = 1.0f;
 bIsTransitioning = false;
        OnTransitionCompleted();
  }

    // Lerp between emotions
  ApplyEmotionVisuals(CurrentEmotion);
}

void UDialogueEmotionWidget::ApplyEmotionVisuals(const FGameplayTag& EmotionTag)
{
    if (!EmotionTag.IsValid())
    {
        return;
    }

    // Set icon
    UTexture2D* Icon = GetEmotionIcon(EmotionTag);
    if (EmotionIcon && Icon)
 {
        EmotionIcon->SetBrushFromTexture(Icon);
    }

    // Set color
    FLinearColor Color = GetEmotionColor(EmotionTag);
    
    if (EmotionIcon)
    {
  EmotionIcon->SetColorAndOpacity(Color);
    }

    if (bEnableGlow && EmotionGlow)
    {
      EmotionGlow->SetColorAndOpacity(Color);
        EmotionGlow->SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    // Update material parameters if available
    if (EmotionMaterial)
    {
        EmotionMaterial->SetVectorParameterValue(FName("EmotionColor"), Color);
        EmotionMaterial->SetScalarParameterValue(FName("EmotionIntensity"), 1.0f);
 }
}

FLinearColor UDialogueEmotionWidget::GetEmotionColor(const FGameplayTag& EmotionTag) const
{
    const FLinearColor* Color = EmotionColors.Find(EmotionTag);
    if (Color)
    {
        return *Color;
    }

    // Default colors based on common emotion types
    FString TagString = EmotionTag.ToString();
    
    if (TagString.Contains(TEXT("Happy")) || TagString.Contains(TEXT("Joy")))
    {
        return FLinearColor::Yellow;
    }
    else if (TagString.Contains(TEXT("Sad")) || TagString.Contains(TEXT("Depressed")))
    {
        return FLinearColor::Blue;
    }
    else if (TagString.Contains(TEXT("Angry")) || TagString.Contains(TEXT("Rage")))
    {
        return FLinearColor::Red;
    }
    else if (TagString.Contains(TEXT("Fear")) || TagString.Contains(TEXT("Scared")))
    {
        return FLinearColor(0.5f, 0.0f, 0.5f); // Purple
    }
    else if (TagString.Contains(TEXT("Surprised")) || TagString.Contains(TEXT("Shock")))
    {
        return FLinearColor(1.0f, 0.5f, 0.0f); // Orange
    }
    else if (TagString.Contains(TEXT("Neutral")))
    {
        return FLinearColor::Gray;
    }

    return FLinearColor::White;
}

UTexture2D* UDialogueEmotionWidget::GetEmotionIcon(const FGameplayTag& EmotionTag) const
{
    TObjectPtr<UTexture2D> const* Icon = EmotionIcons.Find(EmotionTag);
    if (Icon)
    {
return *Icon;
    }

  return nullptr;
}
