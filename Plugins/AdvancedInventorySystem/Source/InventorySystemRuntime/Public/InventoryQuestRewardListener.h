// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryQuestRewardListener.generated.h"

/**
 * Inventory Quest Reward Listener Component
 * 
 * Listens to GameEvent.Quest.GrantItem events from QuestSystem
 * and adds items to the owner's InventoryComponent.
 * 
 * This creates a decoupled integration:
 * QuestSystem -> GameEventBus -> InventorySystem
 * (no hard dependencies between plugins!)
 * 
 * Add this component to your PlayerPawn/PlayerController to enable quest rewards.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEMRUNTIME_API UInventoryQuestRewardListener : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryQuestRewardListener();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Handle for event subscription (for unsubscribing) */
	FDelegateHandle EventSubscriptionHandle;

	/** Event handler for GameEvent.Quest.GrantItem */
	void HandleGrantItemEvent(const struct FGameEventPayload& Payload);

	/** Helper: Find ItemDataAsset by ItemID */
	class UItemDataAsset* FindItemDataAssetByID(FName ItemID) const;
};
