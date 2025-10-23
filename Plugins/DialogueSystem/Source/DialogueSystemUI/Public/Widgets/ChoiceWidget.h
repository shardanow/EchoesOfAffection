// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChoiceWidget.generated.h"

// Forward declarations
class UDialogueNode;
class UButton;
class UTextBlock;
class UImage;
class UWidgetAnimation;
class USoundBase;
class UBorder;
class URichTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelectedSignature, int32, ChoiceIndex);

/**
 * Widget for a single dialogue choice button
 * Displays text, consequence indicators, icons, animations, hotkeys
 * 
 * UX Features:
 * - Hover effects
 * - Keyboard/gamepad support
 * - Visual state indicators (locked, warning, consequence)
 * - Smooth animations
 * - Sound effects
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UChoiceWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    //~ End UUserWidget Interface

    //~ Begin Events

    UPROPERTY(BlueprintAssignable, Category = "Dialogue|Events")
    FOnChoiceSelectedSignature OnChoiceSelected;

    //~ End Events

protected:
    //~ Begin Widget Bindings

    /** Root border */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UBorder> ChoiceBorder;

    /** Button */
  UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> ChoiceButton;

    /** Choice text (RichText for formatting) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URichTextBlock> ChoiceText;

    /** Consequence icon (important consequences, warning, consequence) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> ConsequenceIcon;

  /** Locked icon (for unavailable choices) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> LockedIcon;

    /** Warning icon */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> WarningIcon;

    /** Hotkey text / hotkey (1, 2, 3...) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> HotkeyText;

  /** Hover overlay */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> HoverOverlay;

    //~ End Widget Bindings

    //~ Begin Configuration

    /** Enable effects on hover */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
    bool bEnableHoverEffects = true;

    /** Enable animations */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animation")
    bool bEnableAnimations = true;

    /** Show hotkey text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Input")
    bool bShowHotkeys = true;

    /** Hover sound */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
    TObjectPtr<USoundBase> HoverSound;

    //~ End Configuration

    //~ Begin Animations

    /** Appear animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> AppearAnimation;

    /** Hover animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> HoverAnimation;

    /** Unhover animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> UnhoverAnimation;

    /** Click animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> ClickAnimation;

    /** Disabled animation (for unavailable choices) */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> DisabledAnimation;

    //~ End Animations

    //~ Begin Data

    /** Associated node */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> AssociatedNode;

    /** Index */
    UPROPERTY(Transient)
    int32 ChoiceIndex = -1;

    /** Is choice available */
    UPROPERTY(Transient)
    bool bIsAvailable = true;

    /** Is choice currently hovered */
    UPROPERTY(Transient)
    bool bIsHovered = false;

    //~ End Data

public:
  //~ Begin Public API

    /** Initialize choice with node and index */
UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void InitializeChoice(UDialogueNode* Node, int32 Index);

    /** Set choice data (used by DialogueWidget) */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SetChoiceData(UDialogueNode* Node, int32 Index);

    /** Set hover sound */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SetHoverSound(USoundBase* Sound);

    /** Play appear animation with delay */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void PlayAppearAnimation(float Delay = 0.0f);

    /** Get choice index */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    int32 GetChoiceIndex() const { return ChoiceIndex; }

    /** Get node */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    UDialogueNode* GetNode() const { return AssociatedNode; }

    /** Is choice available */
UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
bool IsAvailable() const { return bIsAvailable; }

    //~ End Public API

protected:
    //~ Begin Event Handlers

    /** Callback: button clicked */
    UFUNCTION()
    void HandleButtonClicked();

    /** Callback: Hover */
    UFUNCTION()
    void HandleButtonHovered();

    /** Callback: Unhover */
    UFUNCTION()
    void HandleButtonUnhovered();

    //~ End Event Handlers

    //~ Begin Internal Logic

    /** Update visuals */
  void UpdateVisuals();

    /** Update availability */
    void UpdateAvailability();

    /** Update state icons */
    void UpdateStateIcons();

 /** Play hover sound */
 void PlayHoverSound();

    //~ End Internal Logic

    //~ Begin Blueprint Events

    /** Blueprint event for initialization */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnChoiceInitialized(UDialogueNode* Node, int32 Index);

    /** Blueprint event for hover */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnChoiceHovered();

    /** Blueprint event for unhover */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnChoiceUnhovered();

    /** Blueprint event for click */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnChoiceClicked();

    //~ End Blueprint Events
};
