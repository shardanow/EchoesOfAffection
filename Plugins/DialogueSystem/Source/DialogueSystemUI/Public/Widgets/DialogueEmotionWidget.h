// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "DialogueEmotionWidget.generated.h"

// Forward declarations
class UImage;
class UWidgetAnimation;
class UTexture2D;
class UMaterialInstanceDynamic;

/**
 * Widget for visualizing character emotions
 * Displays emotions using animated effects
 * 
 * Features:
 * - Smooth emotion transitions
 * - Animated icons
 * - Color tinting based on emotion
 * - Particle effects support (via Blueprint)
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UDialogueEmotionWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    //~ End UUserWidget Interface

protected:
    //~ Begin Widget Bindings

    /** Emotion icon */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> EmotionIcon;

    /** Emotion glow (glow effect) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> EmotionGlow;

    //~ End Widget Bindings

    //~ Begin Configuration

    /** Icon map for each emotion */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion|Icons")
    TMap<FGameplayTag, TObjectPtr<UTexture2D>> EmotionIcons;

    /** Color map for each emotion */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion|Colors")
    TMap<FGameplayTag, FLinearColor> EmotionColors;

    /** Transition duration */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion|Animation")
    float TransitionDuration = 0.3f;

    /** Enable animations */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion|Animation")
    bool bEnableAnimations = true;

    /** Enable glow effects */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emotion|Animation")
    bool bEnableGlow = true;

    //~ End Configuration

    //~ Begin Animations

    /** Emotion change animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
  TObjectPtr<UWidgetAnimation> EmotionChangeAnimation;

    /** Pulse animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
  TObjectPtr<UWidgetAnimation> PulseAnimation;

    /** Appear animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> AppearAnimation;

    /** Disappear animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> DisappearAnimation;

    //~ End Animations

    //~ Begin Runtime State

    /** Current emotion */
    UPROPERTY(Transient)
 FGameplayTag CurrentEmotion;

  /** Previous emotion (for smooth transitions) */
    UPROPERTY(Transient)
    FGameplayTag PreviousEmotion;

    /** Material Instance for dynamic effects */
    UPROPERTY(Transient)
 TObjectPtr<UMaterialInstanceDynamic> EmotionMaterial;

    /** Is currently transitioning */
    UPROPERTY(Transient)
    bool bIsTransitioning = false;

 /** Transition progress */
    UPROPERTY(Transient)
    float TransitionProgress = 0.0f;

    //~ End Runtime State

public:
    //~ Begin Public API

    /** Set emotion */
    UFUNCTION(BlueprintCallable, Category = "Emotion")
    void SetEmotion(const FGameplayTag& EmotionTag, bool bAnimate = true);

    /** Clear emotion */
    UFUNCTION(BlueprintCallable, Category = "Emotion")
    void ClearEmotion(bool bAnimate = true);

    /** Get current emotion */
    UFUNCTION(BlueprintPure, Category = "Emotion")
    FGameplayTag GetCurrentEmotion() const { return CurrentEmotion; }

    /** Check if transitioning */
    UFUNCTION(BlueprintPure, Category = "Emotion")
    bool IsTransitioning() const { return bIsTransitioning; }

    //~ End Public API

protected:
    //~ Begin Internal Logic

    /** Update transition */
    void UpdateTransition(float DeltaTime);

    /** Apply visuals for emotion */
    void ApplyEmotionVisuals(const FGameplayTag& EmotionTag);

    /** Get color for emotion */
    FLinearColor GetEmotionColor(const FGameplayTag& EmotionTag) const;

    /** Get icon for emotion */
    UTexture2D* GetEmotionIcon(const FGameplayTag& EmotionTag) const;

    //~ End Internal Logic

    //~ Begin Blueprint Events

    /** Event when emotion changed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Emotion")
    void OnEmotionChanged(const FGameplayTag& NewEmotion, const FGameplayTag& OldEmotion);

 /** Event when emotion cleared */
    UFUNCTION(BlueprintImplementableEvent, Category = "Emotion")
    void OnEmotionCleared();

    /** Event when transition started (for custom effects) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Emotion")
  void OnTransitionStarted(const FGameplayTag& FromEmotion, const FGameplayTag& ToEmotion);

    /** Event when transition completed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Emotion")
    void OnTransitionCompleted();

    //~ End Blueprint Events
};
