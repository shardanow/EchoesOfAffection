// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/RelationshipWidgetBase.h"
#include "GameplayTagContainer.h"
#include "RelationshipPanel.generated.h"

class UVerticalBox;
class URelationshipDimensionBar;
struct FGameEventPayload;

/**
 * Panel widget displaying all dimensions and state for a relationship
 * Automatically creates dimension bars for all dimensions in the database
 * 
 * Features:
 * - Event-driven updates via GameEventBus (no polling!)
 * - Automatic show/hide on dialogue start/end
 * - Real-time updates on relationship changes
 * - Time-based updates for decay visualization
 */
UCLASS()
class RELATIONSHIPSYSTEMUI_API URelationshipPanel : public URelationshipWidgetBase
{
	GENERATED_BODY()

public:
	/** Vertical container for dimension bars (bind in Blueprint) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UVerticalBox> DimensionContainer;

	/** Dimension bar widget class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Panel")
	TSubclassOf<URelationshipDimensionBar> DimensionBarClass;

	/** Show all dimensions or only non-zero? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Panel")
	bool bShowAllDimensions;

	/** Auto-hide when no dialogue active? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Panel|Events")
	bool bAutoHideWhenNoDialogue = true;

	/** Subscribe to dialogue events? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Panel|Events")
	bool bListenToDialogueEvents = true;

	/** Subscribe to time events for decay? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship Panel|Events")
	bool bListenToTimeEvents = true;

	/** Get current relationship state */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship Panel")
	FGameplayTag GetCurrentState() const;

	/** Get state display name */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationship Panel")
	FText GetStateDisplayName() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeRefreshDisplay() override;

private:
	/** Spawned dimension bar widgets */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URelationshipDimensionBar>> SpawnedDimensionBars;

	/** Event subscription handles */
	FDelegateHandle DialogueStartedHandle;
	FDelegateHandle DialogueEndedHandle;
	FDelegateHandle RelationshipChangedHandle;
	FDelegateHandle TimeHourChangedHandle;

	/** Create dimension bars */
	void CreateDimensionBars();

	/** Clear dimension bars */
	void ClearDimensionBars();

	/** Subscribe to GameEventBus events */
	void SubscribeToEvents();

	/** Unsubscribe from GameEventBus events */
	void UnsubscribeFromEvents();
};
