// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RelationshipNPCListWidget.generated.h"

class UScrollBox;
class URelationshipNPCEntryWidget;
class URelationshipPanel;
struct FRelationshipData;

/** Delegate for NPC selection */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCSelectedSignature, AActor*, SelectedNPC);

/**
 * Widget displaying a list of all NPCs with relationships
 * Allows player to select an NPC to view detailed relationship data
 * 
 * Features:
 * - Auto-populates from RelationshipSubsystem
 * - Filtering by state (Friend, Enemy, etc.)
 * - Sorting by dimension values
 * - Click to show detailed panel
 * - Search functionality
 */
UCLASS()
class RELATIONSHIPSYSTEMUI_API URelationshipNPCListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Scroll container for NPC entries (bind in Blueprint) */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UScrollBox> NPCContainer;

	/** NPC entry widget class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List")
	TSubclassOf<URelationshipNPCEntryWidget> NPCEntryClass;

	/** Relationship panel to show when NPC selected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List")
	TSubclassOf<URelationshipPanel> DetailPanelClass;

	/** Filter by state (empty = show all) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List|Filter")
	FGameplayTag FilterByState;

	/** Filter by minimum dimension value */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List|Filter")
	FGameplayTag FilterDimension;

	/** Minimum value for filter dimension */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List|Filter",
		meta = (EditCondition = "FilterDimension.IsValid()", ClampMin = "0", ClampMax = "100"))
	float MinDimensionValue = 0.0f;

	/** Sort by dimension (empty = no sorting) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List|Sorting")
	FGameplayTag SortByDimension;

	/** Sort ascending or descending? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List|Sorting")
	bool bSortDescending = true;

	/** Auto-refresh when relationships change? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC List")
	bool bAutoRefresh = true;

	/** Populate list from RelationshipSubsystem */
	UFUNCTION(BlueprintCallable, Category = "NPC List")
	void PopulateList();

	/** Clear all entries */
	UFUNCTION(BlueprintCallable, Category = "NPC List")
	void ClearList();

	/** Apply current filters and refresh */
	UFUNCTION(BlueprintCallable, Category = "NPC List")
	void ApplyFilters();

	/** Called when an NPC is selected */
	UFUNCTION(BlueprintCallable, Category = "NPC List")
	void OnNPCSelected(AActor* NPC);

	/** Get player actor (Subject for all relationships) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NPC List")
	AActor* GetPlayerActor() const;

	/** Event when NPC selected */
	UPROPERTY(BlueprintAssignable, Category = "NPC List|Events")
	FOnNPCSelectedSignature OnNPCSelectedEvent;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	/** Spawned NPC entry widgets */
	UPROPERTY(Transient)
	TArray<TObjectPtr<URelationshipNPCEntryWidget>> SpawnedEntries;

	/** Currently displayed detail panel */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipPanel> CurrentDetailPanel;

	/** Subscribe to relationship change events */
	void SubscribeToEvents();

	/** Unsubscribe from events */
	void UnsubscribeFromEvents();

	/** Handle relationship changed event */
	void OnRelationshipChanged(const struct FGameEventPayload& Payload);

	/** Event subscription handle */
	FDelegateHandle RelationshipChangedHandle;

	/** Check if relationship passes filters */
	bool PassesFilters(const FRelationshipData& RelData) const;

	/** Sort relationship data */
	void SortRelationshipData(TArray<FRelationshipData>& RelationshipList) const;
};
