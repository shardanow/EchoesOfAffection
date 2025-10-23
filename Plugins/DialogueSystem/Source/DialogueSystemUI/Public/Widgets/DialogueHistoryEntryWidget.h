// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueHistoryEntryWidget.generated.h"

// Forward declarations
class UTextBlock;
class URichTextBlock;
class UImage;
class UBorder;
class UWidgetAnimation;

/**
 * Widget for a single dialogue history entry
 * Displays speaker name, dialogue text and portrait
 * 
 * Features:
 * - Compact display format
 * - Speaker portrait thumbnail
 * - Fade-in animation
 * - Different styling for player vs NPC
 */
UCLASS(Abstract)
class DIALOGUESYSTEMUI_API UDialogueHistoryEntryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    //~ Begin UUserWidget Interface
    virtual void NativeConstruct() override;
    //~ End UUserWidget Interface

protected:
    //~ Begin Widget Bindings

    /** Root border/container */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UBorder> EntryBorder;

    /** Speaker portrait (thumbnail) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UImage> SpeakerPortrait;

  /** Speaker name */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> SpeakerName;

    /** Dialogue text */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<URichTextBlock> DialogueText;

    /** Timestamp (optional) */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> Timestamp;

    //~ End Widget Bindings

    //~ Begin Animations

    /** Appear animation */
    UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnimOptional))
 TObjectPtr<UWidgetAnimation> FadeInAnimation;

    //~ End Animations

 //~ Begin Data

    /** Is this a player choice */
    UPROPERTY(Transient)
    bool bIsPlayerChoice = false;

    //~ End Data

public:
    //~ Begin Public API

 /** Initialize history entry */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|History")
    void SetHistoryData(const FText& InSpeakerName, const FText& InDialogueText, UTexture2D* InPortrait, bool bInIsPlayerChoice = false);

    /** Set timestamp */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|History")
    void SetTimestamp(const FText& InTimestamp);

  /** Check if this is a player choice */
    UFUNCTION(BlueprintPure, Category = "Dialogue|History")
    bool IsPlayerChoice() const { return bIsPlayerChoice; }

    //~ End Public API

protected:
    /** Update styling based on speaker type */
    void UpdateStyling();

    /** Blueprint event for initialization */
    UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue|History")
    void OnHistoryEntryInitialized(bool bIsPlayer);
};
