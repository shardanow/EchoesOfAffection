// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameEventBusSubsystem.h"
#include "GameEventEffectRegistry.h"
#include "Engine/GameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameEventBus, Log, All);

void UGameEventBusSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogGameEventBus, Log, TEXT("GameEventBusSubsystem: Initialized"));
	
	// Create Effect Registry
	EffectRegistry = NewObject<UGameEventEffectRegistry>(this, TEXT("EffectRegistry"));
	
	TotalEventsEmitted = 0;
	EventHistory.Empty();
}

void UGameEventBusSubsystem::Deinitialize()
{
	UE_LOG(LogGameEventBus, Log, TEXT("GameEventBusSubsystem: Deinitializing (Total events: %d)"), TotalEventsEmitted);
	
	// Clean up registry
	if (EffectRegistry)
	{
		EffectRegistry->ClearAllRegistrations();
		EffectRegistry = nullptr;
	}
	
	ClearAllSubscriptions();
	EventHistory.Empty();
	
	Super::Deinitialize();
}

void UGameEventBusSubsystem::EmitEvent(const FGameEventPayload& Payload, bool bLogEvent)
{
	if (bLogEvent || bEventHistoryEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("[GameEventBus] ?? EmitEvent: '%s' | StringParam='%s' | IntParam=%d | FloatParam=%.2f"),
			*Payload.EventTag.ToString(),
			*Payload.StringParam.ToString(),
			Payload.IntParam,
			Payload.FloatParam);
	}

	// Track statistics
	TotalEventsEmitted++;

	// Store in history if enabled
	if (bEventHistoryEnabled)
	{
		EventHistory.Add(Payload);
		
		// Trim history if too large
		if (EventHistory.Num() > MaxHistorySize)
		{
			EventHistory.RemoveAt(0, EventHistory.Num() - MaxHistorySize);
		}
	}

	// Broadcast to listeners
	BroadcastToListeners(Payload);
	
	UE_LOG(LogTemp, Verbose, TEXT("[GameEventBus] ? Event broadcast complete"));
}

void UGameEventBusSubsystem::SubscribeToEvent(const FGameplayTag& EventTag, const FGameEventDelegate& Listener)
{
	// This method is deprecated for Blueprint usage
	// Use UGameEventBusLibrary::SubscribeToEvent instead
	UE_LOG(LogGameEventBus, Warning, TEXT("SubscribeToEvent: This method should not be called directly from Blueprint. Use UGameEventBusLibrary instead."));
}

void UGameEventBusSubsystem::SubscribeToEventHierarchy(const FGameplayTag& ParentTag, const FGameEventDelegate& Listener)
{
	// This method is deprecated for Blueprint usage
	// Use UGameEventBusLibrary::SubscribeToEventHierarchy instead
	UE_LOG(LogGameEventBus, Warning, TEXT("SubscribeToEventHierarchy: This method should not be called directly from Blueprint. Use UGameEventBusLibrary instead."));
}

void UGameEventBusSubsystem::UnsubscribeFromEvent(const FGameplayTag& EventTag, const FGameEventDelegate& Listener)
{
	// This method is deprecated for Blueprint usage
	UE_LOG(LogGameEventBus, Warning, TEXT("UnsubscribeFromEvent: This method should not be called directly from Blueprint."));
}

void UGameEventBusSubsystem::ClearSubscriptionsForTag(const FGameplayTag& EventTag)
{
	ExactTagListeners.Remove(EventTag);
	NativeExactTagListeners.Remove(EventTag);
	HierarchyListeners.Remove(EventTag);
	NativeHierarchyListeners.Remove(EventTag);

	UE_LOG(LogGameEventBus, Verbose, TEXT("Cleared subscriptions for: %s"), *EventTag.ToString());
}

void UGameEventBusSubsystem::ClearAllSubscriptions()
{
	ExactTagListeners.Empty();
	NativeExactTagListeners.Empty();
	HierarchyListeners.Empty();
	NativeHierarchyListeners.Empty();

	UE_LOG(LogGameEventBus, Log, TEXT("Cleared ALL subscriptions"));
}

void UGameEventBusSubsystem::GetEventStats(int32& OutTotalEventsEmitted, int32& OutActiveSubscriptions) const
{
	OutTotalEventsEmitted = this->TotalEventsEmitted;
	OutActiveSubscriptions = ExactTagListeners.Num() + HierarchyListeners.Num() + 
	       NativeExactTagListeners.Num() + NativeHierarchyListeners.Num();
}

TArray<FGameEventPayload> UGameEventBusSubsystem::GetRecentEvents(int32 Count) const
{
	if (!bEventHistoryEnabled || EventHistory.Num() == 0)
	{
		return TArray<FGameEventPayload>();
	}

	int32 StartIndex = FMath::Max(0, EventHistory.Num() - Count);
	return TArray<FGameEventPayload>(EventHistory.GetData() + StartIndex, EventHistory.Num() - StartIndex);
}

void UGameEventBusSubsystem::SetEventHistoryEnabled(bool bEnabled)
{
	bEventHistoryEnabled = bEnabled;
	
	if (!bEnabled)
	{
		EventHistory.Empty();
	}

	UE_LOG(LogGameEventBus, Log, TEXT("Event history: %s"), bEnabled ? TEXT("ENABLED") : TEXT("DISABLED"));
}

FDelegateHandle UGameEventBusSubsystem::SubscribeToEventNative(const FGameplayTag& EventTag, const FGameEventNativeDelegate::FDelegate& Delegate)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogGameEventBus, Warning, TEXT("SubscribeToEventNative: Invalid EventTag!"));
		return FDelegateHandle();
	}

	FGameEventNativeDelegate& NativeDelegate = NativeExactTagListeners.FindOrAdd(EventTag);
	return NativeDelegate.Add(Delegate);
}

FDelegateHandle UGameEventBusSubsystem::SubscribeToEventHierarchyNative(const FGameplayTag& ParentTag, const FGameEventNativeDelegate::FDelegate& Delegate)
{
	if (!ParentTag.IsValid())
	{
		UE_LOG(LogGameEventBus, Warning, TEXT("SubscribeToEventHierarchyNative: Invalid ParentTag!"));
		return FDelegateHandle();
	}

	FGameEventNativeDelegate& NativeDelegate = NativeHierarchyListeners.FindOrAdd(ParentTag);
	return NativeDelegate.Add(Delegate);
}

void UGameEventBusSubsystem::UnsubscribeNative(const FGameplayTag& EventTag, FDelegateHandle& DelegateHandle)
{
	if (FGameEventNativeDelegate* Delegate = NativeExactTagListeners.Find(EventTag))
	{
		Delegate->Remove(DelegateHandle);
	}

	if (FGameEventNativeDelegate* Delegate = NativeHierarchyListeners.Find(EventTag))
	{
		Delegate->Remove(DelegateHandle);
	}

	DelegateHandle.Reset();
}

UGameEventBusSubsystem* UGameEventBusSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UGameEventBusSubsystem>();
}

void UGameEventBusSubsystem::BroadcastToListeners(const FGameEventPayload& Payload)
{
	// 1. Exact tag listeners (Blueprint)
	if (FGameEventDelegate* ExactDelegate = ExactTagListeners.Find(Payload.EventTag))
	{
		ExactDelegate->Broadcast(Payload);
	}

	// 2. Exact tag listeners (Native)
	if (FGameEventNativeDelegate* NativeDelegate = NativeExactTagListeners.Find(Payload.EventTag))
	{
		NativeDelegate->Broadcast(Payload);
	}

	// 3. Hierarchy listeners (Blueprint) - check if payload tag matches or is child
	for (const TPair<FGameplayTag, FGameEventDelegate>& Pair : HierarchyListeners)
	{
		if (Payload.EventTag.MatchesTag(Pair.Key))
		{
			Pair.Value.Broadcast(Payload);
		}
	}

	// 4. Hierarchy listeners (Native)
	for (const TPair<FGameplayTag, FGameEventNativeDelegate>& Pair : NativeHierarchyListeners)
	{
		if (Payload.EventTag.MatchesTag(Pair.Key))
		{
			Pair.Value.Broadcast(Payload);
		}
	}
}
