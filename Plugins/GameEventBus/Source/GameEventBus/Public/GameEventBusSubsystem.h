// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameEventBusSubsystem.generated.h"

// Forward declarations
class UGameEventEffectRegistry;

/**
 * Generic Game Event Payload
 * 
 */
USTRUCT(BlueprintType)
struct GAMEEVENTBUS_API FGameEventPayload
{
	GENERATED_BODY()

	/** Main event tag (e.g., Quest.Event.ItemAcquired, Dialogue.Event.NodeReached) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	FGameplayTag EventTag;

	/** String parameter (ItemId, NpcId, DialogueId, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	FName StringParam;

	/** Secondary string parameter */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	FName StringParam2;

	/** Integer parameter (Count, Amount, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	int32 IntParam = 0;

	/** Float parameter (Value, Threshold, Progress, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	float FloatParam = 0.0f;

	/** Additional tags (Weather tags, Location tags, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	FGameplayTagContainer AdditionalTags;

	/** Instigator actor (Player, NPC, etc) */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	TWeakObjectPtr<AActor> InstigatorActor;

	/** Target actor */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	TWeakObjectPtr<AActor> TargetActor;

	/** 
	 * NEW v1.13.2: Additional actors involved in event (sequence participants, etc.)
	 * Generic field - can be used by ANY system (not just Dialogue!)
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	TArray<AActor*> AdditionalActors;

	/**
	 * NEW v1.14: Additional PersonaIds (for AdditionalPersonas in DialogueDataAsset)
	 * Used by ScheduleComponent to check if it should pause during dialogue
	 * Format: ["Alice", "Bob", "Eve"]
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	TArray<FString> AdditionalPersonaIds;

	/** Arbitrary UObject payload */
	UPROPERTY(BlueprintReadWrite, Category = "Event")
	TWeakObjectPtr<UObject> PayloadObject;

	/** Timestamp when event was created */
	UPROPERTY(BlueprintReadOnly, Category = "Event")
	FDateTime Timestamp;

	FGameEventPayload()
		: IntParam(0)
		, FloatParam(0.0f)
		, Timestamp(FDateTime::Now())
	{
	}

	/** Convenience constructors */
	FGameEventPayload(const FGameplayTag& InEventTag)
		: EventTag(InEventTag)
		, IntParam(0)
		, FloatParam(0.0f)
		, Timestamp(FDateTime::Now())
	{
	}

	FGameEventPayload(const FGameplayTag& InEventTag, FName InStringParam, int32 InIntParam = 0)
		: EventTag(InEventTag)
		, StringParam(InStringParam)
		, IntParam(InIntParam)
		, FloatParam(0.0f)
		, Timestamp(FDateTime::Now())
	{
	}
};

/**
 * Event Listener Delegate
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameEventDelegate, const FGameEventPayload&, Payload);

/**
 * Native C++ event listener delegate
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FGameEventNativeDelegate, const FGameEventPayload& /*Payload*/);

/**
 * Central Game Event Bus Subsystem
 * 
 * 
 * 
 * - 
 * - )
 * - 
 * - 
 * 
 * 
 * 
 * 1) 
 * 2) 
 * 3) 
 * 4) 
 * 
 * , 
 */
UCLASS(BlueprintType)
class GAMEEVENTBUS_API UGameEventBusSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

	/**
	 * Emit game event (from any system)
	 * 
	 * @param Payload Event data
	 * @param bLogEvent Log event to console (for debugging)
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events", meta = (AutoCreateRefTerm = "Payload"))
	void EmitEvent(const FGameEventPayload& Payload, bool bLogEvent = false);

	/**
	 * Subscribe to specific event tag (exact match)
	 * NOTE: For Blueprint usage, use UGameEventBusLibrary::SubscribeToEvent instead
	 * 
	 * @param EventTag The tag to listen for (e.g., "Quest.Event.ItemAcquired")
	 * @param Listener The delegate to call when event occurs
	 */
	void SubscribeToEvent(const FGameplayTag& EventTag, const FGameEventDelegate& Listener);

	/**
	 * Subscribe to event hierarchy (matches tag and all children)
	 * NOTE: For Blueprint usage, use UGameEventBusLibrary::SubscribeToEventHierarchy instead
	 * 
	 * @param ParentTag Parent tag (e.g., "Quest.Event" will match all Quest.Event.*)
	 * @param Listener The delegate to call when event occurs
	 */
	void SubscribeToEventHierarchy(const FGameplayTag& ParentTag, const FGameEventDelegate& Listener);

	/**
	 * Unsubscribe from event
	 * NOTE: For BP delegates, you must keep reference to the original delegate
	 */
	void UnsubscribeFromEvent(const FGameplayTag& EventTag, const FGameEventDelegate& Listener);

	/**
	 * Clear all subscriptions for a specific tag
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events")
	void ClearSubscriptionsForTag(const FGameplayTag& EventTag);

	/**
	 * Clear ALL subscriptions (use with caution!)
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events")
	void ClearAllSubscriptions();

	/**
	 * Get event statistics (for debugging/profiling)
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Debug")
	void GetEventStats(int32& OutTotalEventsEmitted, int32& OutActiveSubscriptions) const;

	/**
	 * Get recent events (for debugging)
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Debug")
	TArray<FGameEventPayload> GetRecentEvents(int32 Count = 10) const;

	/**
	 * Enable/disable event history tracking
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Events|Debug")
	void SetEventHistoryEnabled(bool bEnabled);

	/**
	 * Get Effect Registry
	 */
	UFUNCTION(BlueprintPure, Category = "Game Events|Registry")
	UGameEventEffectRegistry* GetEffectRegistry() const { return EffectRegistry; }

	//~ Begin C++ API
	/**
	 * Native C++ subscription (more efficient)
	 */
	FDelegateHandle SubscribeToEventNative(const FGameplayTag& EventTag, const FGameEventNativeDelegate::FDelegate& Delegate);
	
	/**
	 * Native C++ hierarchy subscription
	 */
	FDelegateHandle SubscribeToEventHierarchyNative(const FGameplayTag& ParentTag, const FGameEventNativeDelegate::FDelegate& Delegate);

	/**
	 * Unsubscribe native delegate
	 */
	void UnsubscribeNative(const FGameplayTag& EventTag, FDelegateHandle& DelegateHandle);

	/**
	 * Get subsystem instance (convenience)
	 */
	static UGameEventBusSubsystem* Get(const UObject* WorldContextObject);
	//~ End C++ API

protected:
	/** Event listeners (exact tag match) */
	UPROPERTY(Transient)
	TMap<FGameplayTag, FGameEventDelegate> ExactTagListeners;

	/** Native event listeners (exact tag match) */
	TMap<FGameplayTag, FGameEventNativeDelegate> NativeExactTagListeners;

	/** Hierarchy listeners (matches tag and children) */
	UPROPERTY(Transient)
	TMap<FGameplayTag, FGameEventDelegate> HierarchyListeners;

	/** Native hierarchy listeners */
	TMap<FGameplayTag, FGameEventNativeDelegate> NativeHierarchyListeners;

	/** Effect and Condition Registry */
	UPROPERTY(Transient)
	TObjectPtr<UGameEventEffectRegistry> EffectRegistry;

	/** Event history (for debugging) */
	UPROPERTY(Transient)
	TArray<FGameEventPayload> EventHistory;

	/** Total events emitted counter */
	UPROPERTY(Transient)
	int32 TotalEventsEmitted = 0;

	/** Enable event history tracking? */
	UPROPERTY(Transient)
	bool bEventHistoryEnabled = false;

	/** Max history size */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxHistorySize = 100;

private:
	/** Helper: Broadcast to matching listeners */
	void BroadcastToListeners(const FGameEventPayload& Payload);
};
