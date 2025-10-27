// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/NeedsTypes.h"
#include "Data/NeedDefinitionAsset.h"
#include "NeedDisplayWidget.generated.h"

// Forward declarations
class UProgressBar;
class UTextBlock;
class UImage;

/**
 * Widget for displaying a single need
 * Can be used standalone or as part of a larger UI
 * 
 * Blueprint-extensible for custom styling
 */
UCLASS(Blueprintable, BlueprintType)
class NEEDSUI_API UNeedDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ========================================
	// CONFIGURATION
	// ========================================

	/** Need tag to display */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display", meta = (ExposeOnSpawn = "true"))
	FGameplayTag NeedTag;

	/** Need definition (optional, will auto-find if not set) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UNeedDefinitionAsset> NeedDefinition;

	/** Show need name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display")
	bool bShowName = true;

	/** Show need value as text */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display")
	bool bShowValue = true;

	/** Show need icon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display")
	bool bShowIcon = true;

	/** Show threshold indicator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Need Display")
	bool bShowThreshold = true;

	// ========================================
	// WIDGET REFERENCES (Bind in Blueprint)
	// ========================================

	/** Progress bar for need value */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UProgressBar> ValueProgressBar;

	/** Text block for need name */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameTextBlock;

	/** Text block for need value */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ValueTextBlock;

	/** Image for need icon */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	/** Image for threshold indicator */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UImage> ThresholdIndicator;

	// ========================================
	// RUNTIME STATE
	// ========================================

	/** Current need value */
	UPROPERTY(BlueprintReadOnly, Category = "Need Display")
	float CurrentValue = 100.0f;

	/** Current threshold level */
	UPROPERTY(BlueprintReadOnly, Category = "Need Display")
	ENeedThresholdLevel CurrentThreshold = ENeedThresholdLevel::Normal;

	// ========================================
	// UPDATE FUNCTIONS
	// ========================================

	/** Update the display with new value */
	UFUNCTION(BlueprintCallable, Category = "Need Display")
	void UpdateDisplay(float NewValue, ENeedThresholdLevel NewThreshold);

	/** Set need definition (updates icon, name, etc.) */
	UFUNCTION(BlueprintCallable, Category = "Need Display")
	void SetNeedDefinition(UNeedDefinitionAsset* InDefinition);

	// ========================================
	// BLUEPRINT EVENTS
	// ========================================

	/** Called when need value changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Need Display", meta = (DisplayName = "On Value Changed"))
	void BP_OnValueChanged(float OldValue, float NewValue);

	/** Called when threshold changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Need Display", meta = (DisplayName = "On Threshold Changed"))
	void BP_OnThresholdChanged(ENeedThresholdLevel OldThreshold, ENeedThresholdLevel NewThreshold);

	/** Get color for current threshold (can be overridden in BP) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Need Display")
	FLinearColor GetThresholdColor() const;

protected:
	// ========================================
	// WIDGET LIFECYCLE
	// ========================================

	virtual void NativeConstruct() override;

	// ========================================
	// INTERNAL HELPERS
	// ========================================

	/** Update visual elements */
	void UpdateVisuals();

	/** Update progress bar */
	void UpdateProgressBar();

	/** Update text elements */
	void UpdateTextElements();

	/** Update icon */
	void UpdateIcon();

	/** Update threshold indicator */
	void UpdateThresholdIndicator();
};
