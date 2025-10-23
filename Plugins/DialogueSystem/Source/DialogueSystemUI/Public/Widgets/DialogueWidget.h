// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "DialogueWidget.generated.h"

// Forward declarations
class UDialogueRunner;
class UDialogueNode;
class UDialogueSessionContext;
class UTextBlock;
class UImage;
class UVerticalBox;
class UCanvasPanel;
class URichTextBlock;
class UWidgetAnimation;
class USoundBase;
class UScrollBox;
class UBorder;
class UOverlay;

/**
 * Main dialogue UI widget
 * 
 * Principles:
 * - MVVM pattern (ViewModel separated from View)
 * - Event-driven (subscribes to Runner events)
 * - Animated (typewriter, transitions, hover effects)
 * - Modular (Choice buttons are separate widgets)
 * - Dialogue history (Scrollable history box)
 * - Audio system (Sound effects & voice)
 * - Visual indicators (Visual state indicators)
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UDialogueWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
 virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    //~ End UUserWidget Interface

protected:
    //~ Begin Widget Bindings (meta = (BindWidgetOptional))

    /** Root overlay - main container */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UOverlay> RootOverlay;

    /** Speaker portrait panel */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> SpeakerPortrait;

    /** Speaker name */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> SpeakerName;

    /** Dialogue text (RichText for formatting) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URichTextBlock> DialogueText;

    /** Container for choice buttons */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UVerticalBox> ChoicesContainer;

/** ScrollBox for dialogue history */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UScrollBox> HistoryScrollBox;

    /** Border for dialogue box styling */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UBorder> DialogueBoxBorder;

    /** Emotion icon */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> EmotionIcon;

    /** "Continue" indicator - blinking prompt */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> ContinueIndicator;

    /** "Typing..." indicator */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> TypingIndicator;

    /** Skip hint text (Skip) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> SkipHintText;

    //~ End Widget Bindings

    //~ Begin Configuration - Visual

    /** Widget class for choice button */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Classes")
    TSubclassOf<class UChoiceWidget> ChoiceWidgetClass;

    /** Widget class for history entries */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Classes")
    TSubclassOf<class UUserWidget> HistoryEntryWidgetClass;

    /** Show portrait */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display")
    bool bShowPortrait = true;

    /** Show emotions */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display")
    bool bShowEmotions = true;

    /** Show dialogue history */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display")
    bool bShowHistory = true;

    /** Maximum number of entries in history */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display", meta = (ClampMin = "5", ClampMax = "100"))
    int32 MaxHistoryEntries = 20;
    
    /** Always show continue indicator on dialogue nodes (not just End nodes) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display")
    bool bAlwaysShowContinueIndicator = true;
    
    /** Show continue indicator even during typewriter (allows skip) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Display")
    bool bShowContinueIndicatorDuringTypewriter = false;

    //~ End Configuration - Visual

    //~ Begin Configuration - Typewriter

    /** Enable typewriter effect */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Typewriter")
    bool bEnableTypewriter = true;

    /** Characters per second for typewriter */
    UPROPERTY(EditAnywhere, Category = "UI|Typewriter", meta = (ClampMin = "1.0", ClampMax = "200.0"))
    float CharactersPerSecond = 40.0f;

    /** Enable sound effects for typewriter */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Typewriter")
    bool bEnableTypewriterSound = true;

    /** Sound for typewriter */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Typewriter")
    TObjectPtr<USoundBase> TypewriterSound;

    /** Interval between typewriter sounds */
    UPROPERTY(EditAnywhere, Category = "UI|Typewriter", meta = (ClampMin = "0.01", ClampMax = "1.0"))
    float TypewriterSoundInterval = 0.05f;

    /** Internal accumulator for fractional part */
    float TypewriterAccum = 0.0f;
    float TypewriterSoundAccum = 0.0f;

    //~ End Configuration - Typewriter

    //~ Begin Configuration - Animations

    /** Enable animations */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Animations")
    bool bEnableAnimations = true;

    /** Intro animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> IntroAnimation;

    /** Outro animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> OutroAnimation;

    /** New node animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> NewNodeAnimation;

    /** Choice appear animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> ChoiceAppearAnimation;

    /** Continue blink animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> ContinueBlinkAnimation;

    /** Typing indicator animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
    TObjectPtr<UWidgetAnimation> TypingAnimation;

    //~ End Configuration - Animations

 //~ Begin Configuration - Audio

    /** Sound for choice hover */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
    TObjectPtr<USoundBase> ChoiceHoverSound;

    /** Sound for choice selection */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
    TObjectPtr<USoundBase> ChoiceSelectSound;

    /** Sound for dialogue open */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
    TObjectPtr<USoundBase> DialogueOpenSound;

    /** Sound for dialogue close */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Audio")
    TObjectPtr<USoundBase> DialogueCloseSound;

    //~ End Configuration - Audio

    //~ Begin Runtime State

    /** Current Runner */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueRunner> CurrentRunner;

    /** Typewriter state */
    UPROPERTY(Transient)
    FString FullText;

    UPROPERTY(Transient)
    FString VisibleText;

    UPROPERTY(Transient)
    int32 CurrentCharIndex = 0;

    UPROPERTY(Transient)
    bool bIsTyping = false;

    /** List of created choice buttons */
    UPROPERTY(Transient)
    TArray<TObjectPtr<class UChoiceWidget>> ChoiceWidgets;

    /** Current dialogue node */
    UPROPERTY(Transient)
    TObjectPtr<UDialogueNode> CurrentNode;

    /** Waiting for input flag - PROTECTED from Blueprint modifications! */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Dialogue|State")
    bool bWaitingForInput = false;

    /** Flag to prevent clearing choices during HandleChoicesReady */
    UPROPERTY(Transient)
    bool bIsShowingChoices = false;

    //~ End Runtime State

public:
    //~ Begin Public API

    /** Bind to Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void BindToRunner(UDialogueRunner* Runner);

    /** Unbind from Runner */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void UnbindFromRunner();

    /** Skip typewriter */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SkipTypewriter();

    /** Select choice by index */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void SelectChoice(int32 Index);

    /** Continue dialogue after current node */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void ContinueDialogue();

    /** Clear dialogue history */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void ClearHistory();

    /** Check if currently typing */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    bool IsTyping() const { return bIsTyping; }

    /** Check if waiting for input */
    UFUNCTION(BlueprintPure, Category = "Dialogue|UI")
    bool IsWaitingForInput() const { return bWaitingForInput; }

    //~ End Public API

protected:
    //~ Begin Event Handlers

    /** Callback: entered node */
    UFUNCTION()
    void HandleNodeEntered(UDialogueNode* Node);

    /** Callback: choices ready */
  UFUNCTION()
    void HandleChoicesReady(const TArray<UDialogueNode*>& Choices);

    /** Callback: dialogue ended */
    UFUNCTION()
    void HandleDialogueEnded();

    //~ End Event Handlers

    //~ Begin Internal Logic - Display

    /** Update portrait */
  void UpdatePortrait(const UDialogueNode* Node, const UDialogueSessionContext* Context);

    /** Update speaker name */
    void UpdateSpeakerName(const UDialogueNode* Node);

    /** Update emotion */
    void UpdateEmotion(const FGameplayTag& EmotionTag);

    /** Update continue indicator */
  void UpdateContinueIndicator(bool bShow);

    /** Update typing indicator */
    void UpdateTypingIndicator(bool bShow);
    
    /** PROTECTED: Set waiting for input state with logging */
    FORCEINLINE void SetWaitingForInput(bool bValue, const TCHAR* Reason)
    {
        if (bWaitingForInput != bValue)
        {
          UE_LOG(LogTemp, Log, TEXT("DialogueWidget::SetWaitingForInput - Changed from %d to %d (Reason: %s)"), 
                bWaitingForInput, bValue, Reason);
   bWaitingForInput = bValue;
        }
    }

    //~ End Internal Logic - Display

 //~ Begin Internal Logic - Typewriter

    /** Start typewriter effect */
    void StartTypewriter(const FText& Text);

    /** Update typewriter (tick) */
    void UpdateTypewriter(float DeltaTime);

    /** Play typewriter sound */
    void PlayTypewriterSound();

    //~ End Internal Logic - Typewriter

  //~ Begin Internal Logic - Choices

    /** Create choice buttons */
    void CreateChoiceButtons(const TArray<UDialogueNode*>& Choices, const UDialogueSessionContext* Context);

    /** Clear choice buttons */
    void ClearChoiceButtons();

    /** Animate choices appearance */
    void AnimateChoicesAppear();

    //~ End Internal Logic - Choices

    //~ Begin Internal Logic - History

  /** Add entry to history */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|UI")
    void AddToHistory(const FText& InSpeakerName, const FText& InDialogueText, UTexture2D* Portrait);

    /** Scroll history to bottom */
    void ScrollHistoryToBottom();

    //~ End Internal Logic - History

    //~ Begin Blueprint Events

    /** Play intro animation (Blueprint implementable) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayIntroAnimation();

    /** Play outro animation (Blueprint implementable) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayOutroAnimation();

    /** Play new node animation (Blueprint implementable) */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void PlayNewNodeAnimation();

    /** Event when node changed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnNodeChanged(UDialogueNode* NewNode);

    /** Event when emotion changed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnEmotionChanged(const FGameplayTag& EmotionTag);

    /** Event when typewriter completed */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|UI")
    void OnTypewriterCompleted();

    //~ End Blueprint Events
};
