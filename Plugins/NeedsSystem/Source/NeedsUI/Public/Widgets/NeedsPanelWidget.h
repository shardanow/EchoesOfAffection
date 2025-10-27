// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/NeedsInterface.h"
#include "NeedsPanelWidget.generated.h"

// Forward declarations
class UCharacterNeedsComponent;
class UNeedDisplayWidget;
class UPanelWidget;
class UTextBlock;

/**
 * Widget for displaying all needs from a CharacterNeedsComponent
 * Automatically creates NeedDisplayWidgets for each need
 * 
 * Implements INeedsListener to receive updates
 */
UCLASS(Blueprintable, BlueprintType)
class NEEDSUI_API UNeedsPanelWidget : public UUserWidget, public INeedsListener
{
	GENERATED_BODY()

public:
	// ========================================
	// CONFIGURATION
	// ========================================

	/** Target needs component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs Panel", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<UCharacterNeedsComponent> TargetComponent;

	/** Widget class for individual needs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs Panel")
	TSubclassOf<UNeedDisplayWidget> NeedWidgetClass;

	/** Auto-update on value changes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs Panel")
	bool bAutoUpdate = true;

	/** Show mood indicator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Needs Panel")
	bool bShowMood = true;

	// ========================================
	// WIDGET REFERENCES (Bind in Blueprint)
	// ========================================

	/** Panel to contain need widgets */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UPanelWidget> NeedsContainer;

	/** Text block for mood display */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MoodTextBlock;

	// ========================================
	// RUNTIME STATE
	// ========================================

	/** Created need widgets */
	UPROPERTY(BlueprintReadOnly, Category = "Needs Panel")
	TMap<FGameplayTag, TObjectPtr<UNeedDisplayWidget>> NeedWidgets;

	/** Current mood value */
	UPROPERTY(BlueprintReadOnly, Category = "Needs Panel")
	float CurrentMood = 0.0f;

	// ========================================
	// PUBLIC API
	// ========================================

	/** Set target component */
	UFUNCTION(BlueprintCallable, Category = "Needs Panel")
	void SetTargetComponent(UCharacterNeedsComponent* InComponent);

	/** Refresh all displays */
	UFUNCTION(BlueprintCallable, Category = "Needs Panel")
	void RefreshDisplay();

	/** Clear all need widgets */
	UFUNCTION(BlueprintCallable, Category = "Needs Panel")
	void ClearNeedWidgets();

	// ========================================
	// INeedsListener INTERFACE
	// ========================================

	virtual void OnNeedValueChanged_Implementation(const FNeedHandle& NeedHandle, float OldValue, float NewValue) override;
	virtual void OnNeedThresholdChanged_Implementation(const FNeedHandle& NeedHandle, ENeedThresholdLevel OldLevel, ENeedThresholdLevel NewLevel) override;
	virtual void OnMoodChanged_Implementation(float OldMood, float NewMood) override;
	virtual void OnEffectApplied_Implementation(const FActiveNeedEffect& Effect) override;
	virtual void OnEffectExpired_Implementation(const FActiveNeedEffect& Effect) override;
	virtual void OnModifierAdded_Implementation(const FActiveNeedModifier& Modifier) override;
	virtual void OnModifierRemoved_Implementation(const FActiveNeedModifier& Modifier) override;

	// ========================================
	// BLUEPRINT EVENTS
	// ========================================

	/** Called when panel is refreshed */
	UFUNCTION(BlueprintImplementableEvent, Category = "Needs Panel", meta = (DisplayName = "On Panel Refreshed"))
	void BP_OnPanelRefreshed();

	/** Called when mood updates */
	UFUNCTION(BlueprintImplementableEvent, Category = "Needs Panel", meta = (DisplayName = "On Mood Updated"))
	void BP_OnMoodUpdated(float NewMood);

protected:
	// ========================================
	// WIDGET LIFECYCLE
	// ========================================

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// ========================================
	// INTERNAL HELPERS
	// ========================================

	/** Create need widgets */
	void CreateNeedWidgets();

	/** Update mood display */
	void UpdateMoodDisplay();

	/** Register as listener */
	void RegisterAsListener();

	/** Unregister as listener */
	void UnregisterAsListener();
};
