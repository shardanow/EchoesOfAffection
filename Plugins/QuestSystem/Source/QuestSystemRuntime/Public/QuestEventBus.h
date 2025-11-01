// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "QuestTypes.h"
#include "QuestEventBus.generated.h"

// Forward declarations
struct FQuestEventPayload;

// Blueprint event handler delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestEventSignature, const FQuestEventPayload&, Payload);

/**
 * Quest Event Subscription - Tracks a single event subscription
 */
USTRUCT()
struct FQuestEventSubscription
{
	GENERATED_BODY()

	/** Event tag to listen for */
	FGameplayTag EventTag;

	/** Object that owns this subscription */
	TWeakObjectPtr<UObject> Owner;

	/** Function name to call (for Blueprint) */
	FName FunctionName;

	/** Filter tags (event must have these) */
	FGameplayTagContainer FilterTags;

	/** Filter by string param */
	FName FilterStringParam;

	/** Is active */
	bool bIsActive = true;

	FQuestEventSubscription() 
		: FunctionName(NAME_None)
		, FilterStringParam(NAME_None)
		, bIsActive(true)
	{}
};

/**
 * Quest Event Bus - Central event dispatcher for quest system
 * Event-driven architecture: no ticks, only subscriptions and emissions
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestEventBus : public UObject
{
	GENERATED_BODY()

public:
	UQuestEventBus();

	/**
	 * Initialize with QuestProgressManager reference
	 */
	void Initialize(class UQuestProgressManager* InProgressManager);

	/**
	 * Emit quest event - Send event to all subscribers
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Payload"))
	void EmitEvent(const FQuestEventPayload& Payload);

	/**
	 * Subscribe to quest event
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void Subscribe(FGameplayTag EventTag, UObject* Owner, FName FunctionName);

	/**
	 * Unsubscribe from quest event
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void Unsubscribe(FGameplayTag EventTag, UObject* Owner);

	/**
	 * Unsubscribe all events for owner
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void UnsubscribeAll(UObject* Owner);

	/**
	 * Check if has any subscribers for event
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	bool HasSubscribers(FGameplayTag EventTag) const;

	/**
	 * Get number of active subscriptions
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	int32 GetSubscriptionCount() const;

	/**
	 * Clear all subscriptions
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void ClearAllSubscriptions();

	/**
	 * Enable/disable event bus
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void SetEnabled(bool bEnabled);

	/**
	 * Is event bus enabled
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	bool IsEnabled() const { return bIsEnabled; }

	// Blueprint-callable event emitters for common events

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitItemAcquired(FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitItemUsed(FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitNpcTalked(FName NpcId, AActor* Player, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitDialogChoice(FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitEnterArea(FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "Tags"))
	void EmitActorKilled(AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void EmitTimeReached(int32 Hour, int32 Minute);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void EmitDayChanged(int32 DayNumber);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events", meta = (AutoCreateRefTerm = "WeatherTags"))
	void EmitWeatherChanged(const FGameplayTagContainer& WeatherTags);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void EmitNeedChanged(FGameplayTag NeedTag, float NewValue, float OldValue);

	UFUNCTION(BlueprintCallable, Category = "Quest|Events")
	void EmitAffectionChanged(FName NpcId, float NewValue, float OldValue);

protected:
	/** All subscriptions for management */
	UPROPERTY()
	TArray<FQuestEventSubscription> Subscriptions;

	/** Reference to QuestProgressManager for automatic quest updates */
	UPROPERTY()
	TWeakObjectPtr<class UQuestProgressManager> ProgressManager;

	/** Is event bus enabled */
	bool bIsEnabled = true;

	/** Pending events (for deferred processing) */
	TArray<FQuestEventPayload> PendingEvents;

	/** Is currently processing events */
	bool bIsProcessingEvents = false;

	/** Process deferred events */
	void ProcessPendingEvents();

	/** Check if event passes subscription filter */
	bool PassesFilter(const FQuestEventPayload& Payload, const FQuestEventSubscription& Subscription) const;

	/**
	 * Broadcast event to active quests for automatic objective updates
	 * This is the core method that makes quests auto-update on events!
	 */
	void BroadcastEvent(const FQuestEventPayload& Payload);

	/**
	 * Check if an objective condition matches the incoming event
	 */
	bool DoesConditionMatchEvent(const struct FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;

	/**
	 * Calculate how much progress this event should contribute
	 */
	int32 CalculateProgressAmount(const struct FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const;
};
