// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameEventBusLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameEventBusLibrary, Log, All);

void UGameEventBusLibrary::EmitSimpleEvent(UObject* WorldContextObject, const FGameplayTag& EventTag, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus)
	{
		UE_LOG(LogGameEventBusLibrary, Error, TEXT("[EmitSimpleEvent] ? EventBus is NULL!"));
		return;
	}

	FGameEventPayload Payload(EventTag);
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitEventWithString(UObject* WorldContextObject, const FGameplayTag& EventTag, FName StringParam, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload(EventTag, StringParam);
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitEventWithInt(UObject* WorldContextObject, const FGameplayTag& EventTag, int32 IntParam, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload(EventTag);
	Payload.IntParam = IntParam;
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitEventWithActors(UObject* WorldContextObject, const FGameplayTag& EventTag, AActor* InstigatorActor, AActor* TargetActor, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload(EventTag);
	Payload.InstigatorActor = InstigatorActor;
	Payload.TargetActor = TargetActor;
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitItemEvent(UObject* WorldContextObject, const FGameplayTag& EventTag, FName ItemId, int32 Count, AActor* Instigator, bool bLogEvent)
{
	UE_LOG(LogGameEventBusLibrary, Log, TEXT("[EmitItemEvent] ?? Called! EventTag='%s', ItemId='%s', Count=%d"),
		*EventTag.ToString(),
		*ItemId.ToString(),
		Count);

	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus)
	{
		UE_LOG(LogGameEventBusLibrary, Error, TEXT("[EmitItemEvent] ? EventBus is NULL! WorldContextObject=%s"),
			WorldContextObject ? *WorldContextObject->GetName() : TEXT("NULL"));
		return;
	}

	UE_LOG(LogGameEventBusLibrary, Log, TEXT("[EmitItemEvent] ? EventBus found, creating payload..."));

	FGameEventPayload Payload = MakeItemPayload(EventTag, ItemId, Count, Instigator);
	
	UE_LOG(LogGameEventBusLibrary, Log, TEXT("[EmitItemEvent] ?? Calling EventBus->EmitEvent..."));
	EventBus->EmitEvent(Payload, true); // ? 
	
	UE_LOG(LogGameEventBusLibrary, Log, TEXT("[EmitItemEvent] ? EmitEvent call complete!"));
}

void UGameEventBusLibrary::EmitDialogueEvent(UObject* WorldContextObject, const FGameplayTag& EventTag, FName DialogueId, FName NodeId, AActor* Player, AActor* NPC, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload = MakeDialoguePayload(EventTag, DialogueId, NodeId, Player, NPC);
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitQuestEvent(UObject* WorldContextObject, const FGameplayTag& EventTag, FName QuestId, FName ObjectiveId, int32 Progress, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload = MakeQuestPayload(EventTag, QuestId, ObjectiveId, Progress);
	EventBus->EmitEvent(Payload, bLogEvent);
}

void UGameEventBusLibrary::EmitTimeEvent(UObject* WorldContextObject, const FGameplayTag& EventTag, int32 Hour, int32 Day, bool bLogEvent)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return;

	FGameEventPayload Payload = MakeTimePayload(EventTag, Hour, Day);
	EventBus->EmitEvent(Payload, bLogEvent);
}

UGameEventEffectRegistry* UGameEventBusLibrary::GetEffectRegistry(UObject* WorldContextObject)
{
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(WorldContextObject);
	if (!EventBus) return nullptr;

	return EventBus->GetEffectRegistry();
}

bool UGameEventBusLibrary::ExecuteRegisteredEffect(UObject* WorldContextObject, const FGameplayTag& EffectTag, const FGameEventPayload& Payload)
{
	UGameEventEffectRegistry* Registry = GetEffectRegistry(WorldContextObject);
	if (!Registry) return false;

	return Registry->ExecuteEffect(EffectTag, WorldContextObject, Payload);
}

bool UGameEventBusLibrary::EvaluateRegisteredCondition(UObject* WorldContextObject, const FGameplayTag& ConditionTag, const FGameEventPayload& Payload)
{
	UGameEventEffectRegistry* Registry = GetEffectRegistry(WorldContextObject);
	if (!Registry) return false;

	return Registry->EvaluateCondition(ConditionTag, WorldContextObject, Payload);
}

FGameEventPayload UGameEventBusLibrary::MakeItemPayload(const FGameplayTag& EventTag, FName ItemId, int32 Count, AActor* Instigator)
{
	FGameEventPayload Payload(EventTag);
	Payload.StringParam = ItemId;
	Payload.IntParam = Count;
	Payload.InstigatorActor = Instigator;
	return Payload;
}

FGameEventPayload UGameEventBusLibrary::MakeDialoguePayload(const FGameplayTag& EventTag, FName DialogueId, FName NodeId, AActor* Player, AActor* NPC)
{
	FGameEventPayload Payload(EventTag);
	Payload.StringParam = DialogueId;
	Payload.StringParam2 = NodeId;
	Payload.InstigatorActor = Player;
	Payload.TargetActor = NPC;
	return Payload;
}

FGameEventPayload UGameEventBusLibrary::MakeQuestPayload(const FGameplayTag& EventTag, FName QuestId, FName ObjectiveId, int32 Progress)
{
	FGameEventPayload Payload(EventTag);
	Payload.StringParam = QuestId;
	Payload.StringParam2 = ObjectiveId;
	Payload.IntParam = Progress;
	return Payload;
}

FGameEventPayload UGameEventBusLibrary::MakeTimePayload(const FGameplayTag& EventTag, int32 Hour, int32 Day)
{
	FGameEventPayload Payload(EventTag);
	Payload.IntParam = Hour;
	Payload.FloatParam = static_cast<float>(Day);
	return Payload;
}

UGameEventBusSubsystem* UGameEventBusLibrary::GetGameEventBus(UObject* WorldContextObject)
{
	return UGameEventBusSubsystem::Get(WorldContextObject);
}
