// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/QuestEventBridge.h"
#include "QuestSubsystem.h"
#include "QuestEventBus.h"
#include "QuestTypes.h"
#include "GameEventBusSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogQuestBridge, Log, All);

void UQuestEventBridge::Initialize(UObject* InQuestSubsystem)
{
	if (bIsActive)
	{
		UE_LOG(LogQuestBridge, Warning, TEXT("QuestEventBridge already initialized"));
		return;
	}

	QuestSubsystemRef = InQuestSubsystem;

	// Get GameEventBus
	UWorld* World = InQuestSubsystem->GetWorld();
	if (!World)
	{
		UE_LOG(LogQuestBridge, Error, TEXT("Failed to get World from QuestSubsystem"));
		return;
	}

	EventBusRef = UGameEventBusSubsystem::Get(InQuestSubsystem);
	if (!EventBusRef.IsValid())
	{
		UE_LOG(LogQuestBridge, Error, TEXT("GameEventBusSubsystem not found! Make sure GameEventBus plugin is enabled."));
		return;
	}

	// Subscribe to event hierarchies
	SubscribeToEventTags();

	bIsActive = true;
	UE_LOG(LogQuestBridge, Log, TEXT("QuestEventBridge initialized successfully"));
}

void UQuestEventBridge::Shutdown()
{
	if (!bIsActive) return;

	// Unsubscribe from all events
	if (EventBusRef.IsValid())
	{
		for (FDelegateHandle& Handle : DelegateHandles)
		{
			// We need the tag to unsubscribe, but for simplicity we'll just let subsystem cleanup
			// In production, you'd store tag-handle pairs
		}
	}

	DelegateHandles.Empty();
	EventBusRef.Reset();
	QuestSubsystemRef.Reset();

	bIsActive = false;
	UE_LOG(LogQuestBridge, Log, TEXT("QuestEventBridge shutdown"));
}

void UQuestEventBridge::OnGameEvent(const FGameEventPayload& Payload)
{
	if (!QuestSubsystemRef.IsValid())
	{
		UE_LOG(LogQuestBridge, Warning, TEXT("? QuestSubsystem reference lost"));
		return;
	}

	UE_LOG(LogQuestBridge, Log, TEXT("?? Received game event: '%s' | StringParam='%s' | IntParam=%d"), 
		*Payload.EventTag.ToString(),
		*Payload.StringParam.ToString(),
		Payload.IntParam);

	// Forward to quest system
	ForwardToQuestSystem(Payload);
}

void UQuestEventBridge::ForwardToQuestSystem(const FGameEventPayload& GamePayload)
{
	UQuestSubsystem* QuestSubsystem = Cast<UQuestSubsystem>(QuestSubsystemRef.Get());
	if (!QuestSubsystem)
	{
		UE_LOG(LogQuestBridge, Error, TEXT("? Failed to cast QuestSubsystem!"));
		return;
	}

	UE_LOG(LogQuestBridge, Log, TEXT("?? ForwardToQuestSystem: Converting GameEventPayload to QuestEventPayload..."));

	// Convert GameEventPayload to QuestEventPayload and forward to QuestEventBus
	// QuestEventBus will automatically route events to matching quest objectives!
	FQuestEventPayload QuestPayload;
	QuestPayload.EventTag = GamePayload.EventTag;
	QuestPayload.StringParam = GamePayload.StringParam;
	QuestPayload.IntParam = GamePayload.IntParam;
	QuestPayload.FloatParam = GamePayload.FloatParam;
	QuestPayload.TagsParam = GamePayload.AdditionalTags;
	QuestPayload.InstigatorActor = GamePayload.InstigatorActor;
	QuestPayload.TargetActor = GamePayload.TargetActor;

	UE_LOG(LogQuestBridge, Log, TEXT("?? Emitting to QuestSubsystem: EventTag='%s', StringParam='%s', IntParam=%d"),
		*QuestPayload.EventTag.ToString(),
		*QuestPayload.StringParam.ToString(),
		QuestPayload.IntParam);

	QuestSubsystem->EmitQuestEvent(QuestPayload);

	UE_LOG(LogQuestBridge, Verbose, TEXT("? Event forwarded successfully"));
}

void UQuestEventBridge::SubscribeToEventTags()
{
	if (!EventBusRef.IsValid())
	{
		UE_LOG(LogQuestBridge, Error, TEXT("? EventBusRef is invalid!"));
		return;
	}

	UE_LOG(LogQuestBridge, Log, TEXT("?? SubscribeToEventTags: Starting subscription..."));

	// Subscribe to major event hierarchies that quests might care about
	// Using hierarchy subscription so we catch all sub-events

	// 1. NEW: Subscribe to GameEvent.Inventory.* (unified namespace)
	FGameplayTag GameEventInventoryTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Inventory"), false);
	if (GameEventInventoryTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			GameEventInventoryTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Log, TEXT("? Subscribed to hierarchy: GameEvent.Inventory"));
	}
	else
	{
		UE_LOG(LogQuestBridge, Warning, TEXT("?? GameEvent.Inventory tag not found!"));
	}

	// 2. Subscribe to GameEvent.Quest.* (unified namespace)
	FGameplayTag GameEventQuestTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest"), false);
	if (GameEventQuestTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			GameEventQuestTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Log, TEXT("? Subscribed to hierarchy: GameEvent.Quest"));
	}

	// 3. Subscribe to GameEvent.Dialogue.* (unified namespace)
	FGameplayTag GameEventDialogueTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue"), false);
	if (GameEventDialogueTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			GameEventDialogueTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Log, TEXT("? Subscribed to hierarchy: GameEvent.Dialogue"));
	}

	// 4. OLD: Keep legacy tags for backward compatibility
	// Item events (Inventory.Event.*)
	FGameplayTag ItemEventTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Event"), false);
	if (ItemEventTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			ItemEventTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Verbose, TEXT("Subscribed to legacy: Inventory.Event"));
	}

	// Dialogue events (Dialogue.Event.*)
	FGameplayTag DialogueEventTag = FGameplayTag::RequestGameplayTag(FName("Dialogue.Event"), false);
	if (DialogueEventTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			DialogueEventTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Verbose, TEXT("Subscribed to legacy: Dialogue.Event"));
	}

	// Time events (Time.Event.*)
	FGameplayTag TimeEventTag = FGameplayTag::RequestGameplayTag(FName("Time.Event"), false);
	if (TimeEventTag.IsValid())
	{
		FDelegateHandle Handle = EventBusRef->SubscribeToEventHierarchyNative(
			TimeEventTag,
			FGameEventNativeDelegate::FDelegate::CreateUObject(this, &UQuestEventBridge::OnGameEvent)
		);
		DelegateHandles.Add(Handle);
		UE_LOG(LogQuestBridge, Verbose, TEXT("Subscribed to legacy: Time.Event"));
	}

	UE_LOG(LogQuestBridge, Log, TEXT("?? SubscribeToEventTags: Complete! Total subscriptions: %d"), DelegateHandles.Num());
}
