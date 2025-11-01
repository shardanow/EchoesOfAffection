// Copyright Epic Games, Inc. All Rights Reserved.

#include "Integration/QuestDialogueEffects.h"
#include "QuestSubsystem.h"
#include "QuestEventBus.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

// ========== UQuestDialogueEffect_StartQuest ==========

void UQuestDialogueEffect_StartQuest::Execute(UObject* WorldContextObject)
{
    if (QuestId.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestDialogueEffect_StartQuest: QuestId is None!"));
   return;
    }

    UQuestSubsystem* QuestSys = GetQuestSubsystem(WorldContextObject);
    if (!QuestSys)
  {
   UE_LOG(LogTemp, Error, TEXT("QuestDialogueEffect_StartQuest: Cannot get QuestSubsystem!"));
        return;
    }

    // Start quest asynchronously
    FOnQuestStartedDelegate Callback;
    Callback.BindDynamic(this, &UQuestDialogueEffect_StartQuest::OnQuestStartedCallback);

    QuestSys->StartQuestAsync(QuestId, Callback);
}

void UQuestDialogueEffect_StartQuest::OnQuestStartedCallback(bool bSuccess, UQuestAsset* QuestAsset)
{
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Quest '%s' started from dialogue"), *QuestId.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to start quest '%s' from dialogue"), *QuestId.ToString());
    }
}

FText UQuestDialogueEffect_StartQuest::GetDisplayText() const
{
    return FText::Format(
        FText::FromString(TEXT("Start Quest: {0}")),
        FText::FromName(QuestId)
    );
}

UQuestSubsystem* UQuestDialogueEffect_StartQuest::GetQuestSubsystem(UObject* WorldContextObject) const
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

    return GameInstance->GetSubsystem<UQuestSubsystem>();
}

// ========== UQuestDialogueEffect_CompleteObjective ==========

void UQuestDialogueEffect_CompleteObjective::Execute(UObject* WorldContextObject)
{
    if (QuestId.IsNone() || ObjectiveId.IsNone())
    {
    UE_LOG(LogTemp, Warning, TEXT("QuestDialogueEffect_CompleteObjective: QuestId or ObjectiveId is None!"));
        return;
    }

    UQuestSubsystem* QuestSys = GetQuestSubsystem(WorldContextObject);
    if (!QuestSys)
    {
   UE_LOG(LogTemp, Error, TEXT("QuestDialogueEffect_CompleteObjective: Cannot get QuestSubsystem!"));
   return;
    }

 bool bSuccess = QuestSys->CompleteObjective(QuestId, ObjectiveId);
    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("Quest '%s' objective '%s' completed from dialogue"),
     *QuestId.ToString(), *ObjectiveId.ToString());
    }
}

FText UQuestDialogueEffect_CompleteObjective::GetDisplayText() const
{
    return FText::Format(
        FText::FromString(TEXT("Complete Objective: {0}.{1}")),
     FText::FromName(QuestId),
        FText::FromName(ObjectiveId)
    );
}

UQuestSubsystem* UQuestDialogueEffect_CompleteObjective::GetQuestSubsystem(UObject* WorldContextObject) const
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

    return GameInstance->GetSubsystem<UQuestSubsystem>();
}

// ========== UQuestDialogueEffect_EmitEvent ==========

void UQuestDialogueEffect_EmitEvent::Execute(UObject* WorldContextObject, AActor* Instigator)
{
    if (EventType.IsNone())
    {
        UE_LOG(LogTemp, Warning, TEXT("QuestDialogueEffect_EmitEvent: EventType is None!"));
        return;
    }

    UQuestSubsystem* QuestSys = GetQuestSubsystem(WorldContextObject);
    if (!QuestSys || !QuestSys->GetEventBus())
    {
        UE_LOG(LogTemp, Error, TEXT("QuestDialogueEffect_EmitEvent: Cannot get QuestSubsystem or EventBus!"));
   return;
    }

    UQuestEventBus* EventBus = QuestSys->GetEventBus();

    // Emit different event types
    if (EventType == FName("NpcTalked"))
    {
    EventBus->EmitNpcTalked(NpcId, Instigator, FGameplayTagContainer());
        UE_LOG(LogTemp, Log, TEXT("Emitted NpcTalked event for NPC '%s' from dialogue"), *NpcId.ToString());
    }
    else if (EventType == FName("ItemAcquired"))
    {
        EventBus->EmitItemAcquired(ItemId, Count, Instigator, FGameplayTagContainer());
        UE_LOG(LogTemp, Log, TEXT("Emitted ItemAcquired event for Item '%s' (x%d) from dialogue"),
            *ItemId.ToString(), Count);
    }
    else if (EventType == FName("ItemUsed"))
  {
        EventBus->EmitItemUsed(ItemId, Instigator, nullptr, FGameplayTagContainer());
        UE_LOG(LogTemp, Log, TEXT("Emitted ItemUsed event for Item '%s' from dialogue"), *ItemId.ToString());
    }
    else
    {
      // Custom event
        FQuestEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName(*EventType.ToString()));
        Payload.StringParam = NpcId;
        Payload.IntParam = Count;
        // Note: FQuestEventPayload doesn't have Instigator field

        EventBus->EmitEvent(Payload);
     UE_LOG(LogTemp, Log, TEXT("Emitted custom quest event '%s' from dialogue"), *EventType.ToString());
    }
}

FText UQuestDialogueEffect_EmitEvent::GetDisplayText() const
{
    if (EventType == FName("NpcTalked"))
    {
        return FText::Format(
 FText::FromString(TEXT("Quest Event: NPC Talked ({0})")),
            FText::FromName(NpcId)
        );
    }
    else if (EventType == FName("ItemAcquired"))
    {
        return FText::Format(
            FText::FromString(TEXT("Quest Event: Item Acquired ({0} x{1})")),
            FText::FromName(ItemId),
      FText::AsNumber(Count)
        );
    }
    else
    {
    return FText::Format(
            FText::FromString(TEXT("Quest Event: {0}")),
 FText::FromName(EventType)
        );
    }
}

UQuestSubsystem* UQuestDialogueEffect_EmitEvent::GetQuestSubsystem(UObject* WorldContextObject) const
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

    return GameInstance->GetSubsystem<UQuestSubsystem>();
}
