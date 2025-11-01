// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestEventBus.h"
#include "QuestProgressManager.h"
#include "QuestAsset.h"
#include "QuestSaveGame.h"
#include "QuestActorComponent.h"
#include "Engine/World.h"

UQuestEventBus::UQuestEventBus()
{
	bIsEnabled = true;
	bIsProcessingEvents = false;
}

void UQuestEventBus::Initialize(UQuestProgressManager* InProgressManager)
{
	ProgressManager = InProgressManager;
	
	UE_LOG(LogTemp, Log, TEXT("[QuestEventBus] Initialized with ProgressManager: %s"), 
		ProgressManager.IsValid() ? TEXT("Valid") : TEXT("INVALID"));
}

void UQuestEventBus::EmitEvent(const FQuestEventPayload& Payload)
{
	if (!bIsEnabled)
	{
		return;
	}

	// Prevent recursive event processing
	if (bIsProcessingEvents)
	{
		PendingEvents.Add(Payload);
		return;
	}

	bIsProcessingEvents = true;

	// Log event
	UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus] EmitEvent: %s | StringParam=%s | IntParam=%d"),
		*Payload.EventTag.ToString(),
		*Payload.StringParam.ToString(),
		Payload.IntParam);

	// 1) Broadcast to quest system for automatic objective updates
	BroadcastEvent(Payload);

	// 2) Notify all subscribers
	for (const FQuestEventSubscription& Sub : Subscriptions)
	{
		if (Sub.EventTag.MatchesTag(Payload.EventTag) && PassesFilter(Payload, Sub))
		{
			if (UObject* Owner = Sub.Owner.Get())
			{
				if (UFunction* Func = Owner->FindFunction(Sub.FunctionName))
				{
					Owner->ProcessEvent(Func, (void*)&Payload);
				}
			}
		}
	}

	bIsProcessingEvents = false;

	// Process any events that were queued during processing
	if (PendingEvents.Num() > 0)
	{
		ProcessPendingEvents();
	}
}

void UQuestEventBus::BroadcastEvent(const FQuestEventPayload& Payload)
{
	if (!ProgressManager.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestEventBus::BroadcastEvent] ? NO ProgressManager reference!"));
		return;
	}

	UQuestProgressManager* Manager = ProgressManager.Get();
	
	// Optional validation: Check if Instigator has QuestActorComponent and is allowed to emit events
	if (Payload.InstigatorActor.IsValid()) // FIXED: use IsValid() instead of implicit bool
	{
		UQuestActorComponent* QuestActor = Payload.InstigatorActor->FindComponentByClass<UQuestActorComponent>();
		if (QuestActor && !QuestActor->bCanEmitQuestEvents)
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent] ?? Actor '%s' has QuestActorComponent but bCanEmitQuestEvents=false - ignoring event"),
				*Payload.InstigatorActor->GetName());
			return;
		}
	}
	
	// Get all active quests
	TArray<FQuestSaveData> ActiveQuests = Manager->GetActiveQuests();
	
	UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent] ?? Processing event '%s' (StringParam='%s', IntParam=%d) for %d active quests"),
		*Payload.EventTag.ToString(),
		*Payload.StringParam.ToString(),
		Payload.IntParam,
		ActiveQuests.Num());
	
	if (ActiveQuests.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent] ?? No active quests to update!"));
		return; // No active quests to update
	}

	int32 TotalObjectivesChecked = 0;
	int32 TotalConditionsChecked = 0;
	int32 TotalMatches = 0;

	// For each active quest
	for (const FQuestSaveData& QuestData : ActiveQuests)
	{
		UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent]   ?? Checking quest '%s' (State=%d, CurrentPhase='%s')"),
			*QuestData.QuestId.ToString(),
			(int32)QuestData.State,
			*QuestData.CurrentPhaseId.ToString());

		UQuestAsset* QuestAsset = Manager->GetQuestAsset(QuestData.QuestId);
		if (!QuestAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent]   ? Quest asset NOT FOUND for '%s'"), *QuestData.QuestId.ToString());
			continue;
		}

		// Get current phase
		const FQuestPhase* CurrentPhase = nullptr;
		for (const FQuestPhase& Phase : QuestAsset->Phases)
		{
			if (Phase.PhaseId == QuestData.CurrentPhaseId)
			{
				CurrentPhase = &Phase;
				break;
			}
		}

		if (!CurrentPhase)
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent]     ? Current phase '%s' NOT FOUND in quest asset"), *QuestData.CurrentPhaseId.ToString());
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent]     ? Found current phase '%s' with %d objectives"),
			*CurrentPhase->PhaseId.ToString(),
			CurrentPhase->Objectives.Num());

		// Check each objective in current phase
		for (const FQuestObjective& Objective : CurrentPhase->Objectives)
		{
			TotalObjectivesChecked++;

			// Skip if objective already completed
			int32 CurrentCount, TargetCount;
			bool bIsCompleted;
			Manager->GetObjectiveProgress(QuestData.QuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);
			
			if (bIsCompleted)
			{
				UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::BroadcastEvent]   ?? Objective '%s' already completed - skipping"), *Objective.ObjectiveId.ToString());
				continue; // Already done
			}

			UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent] ?? Checking objective '%s' (Progress: %d/%d, Conditions: %d)"),
				*Objective.ObjectiveId.ToString(),
				CurrentCount,
				TargetCount,
				Objective.Conditions.Num());

			// Check each condition in the objective
			for (const FObjectiveCondition& Condition : Objective.Conditions)
			{
				TotalConditionsChecked++;

				UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent]         ?? Condition: EventTag='%s', ItemId='%s', NpcId='%s', Count=%d"),
					*Condition.EventTag.ToString(),
					*Condition.ItemId.ToString(),
					*Condition.NpcId.ToString(),
					Condition.Count);

				// Does this condition match the event?
				if (DoesConditionMatchEvent(Condition, Payload))
				{
					TotalMatches++;

					// Calculate progress amount
					int32 ProgressAmount = CalculateProgressAmount(Condition, Payload);
					
					if (ProgressAmount > 0)
					{
						// Update the objective!
						UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent]         ??? MATCH FOUND! Event '%s' matched objective '%s' in quest '%s' - Adding %d progress"),
							*Payload.EventTag.ToString(),
							*Objective.ObjectiveId.ToString(),
							*QuestData.QuestId.ToString(),
							ProgressAmount);

						Manager->UpdateObjectiveProgress(QuestData.QuestId, Objective.ObjectiveId, ProgressAmount);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::BroadcastEvent]   ?? Match found but ProgressAmount is 0!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::BroadcastEvent]         ? Condition does NOT match event"));
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::BroadcastEvent] ?? Summary: Checked %d objectives, %d conditions, found %d matches"),
		TotalObjectivesChecked,
		TotalConditionsChecked,
		TotalMatches);
}

bool UQuestEventBus::DoesConditionMatchEvent(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const
{
	UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent] Comparing:"));
	UE_LOG(LogTemp, Verbose, TEXT("  Condition.EventTag = '%s' (Valid: %s)"), 
		*Condition.EventTag.ToString(), 
		Condition.EventTag.IsValid() ? TEXT("YES") : TEXT("NO"));
	UE_LOG(LogTemp, Verbose, TEXT("  Payload.EventTag   = '%s'"), *Payload.EventTag.ToString());

	// EventTag MUST match (STRICT check - no wildcards!)
	if (Condition.EventTag.IsValid())
	{
		// Use exact match instead of MatchesTag to prevent hierarchy matching
		bool bTagMatches = (Payload.EventTag == Condition.EventTag);
		
		UE_LOG(LogTemp, Verbose, TEXT("  EventTag Match? %s (Exact comparison)"), bTagMatches ? TEXT("YES") : TEXT("NO"));
		
		if (!bTagMatches)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent] ? EventTag mismatch - rejecting"));
			return false; // Tag mismatch
		}
	}
	else
	{
		// CHANGED: Empty EventTag is NOT allowed - reject!
		UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::DoesConditionMatchEvent] ?? Condition.EventTag is EMPTY! This is likely a configuration error in the Quest Data Asset."));
		UE_LOG(LogTemp, Warning, TEXT("[QuestEventBus::DoesConditionMatchEvent] ? Rejecting because empty EventTag would match ANY event!"));
		return false; // Empty tag not allowed
	}

	// If ItemId is specified in condition, check string param
	if (!Condition.ItemId.IsNone())
	{
		// Try to get ItemId from QuestActorComponent if payload has Target actor
		FName ActualItemId = Payload.StringParam;
		
		if (ActualItemId.IsNone() && Payload.TargetActor.IsValid()) // FIXED: use IsValid()
		{
			UQuestActorComponent* QuestActor = Payload.TargetActor->FindComponentByClass<UQuestActorComponent>();
			if (QuestActor)
			{
				ActualItemId = QuestActor->GetQuestActorId();
				UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent]   ? Got ItemId from TargetActor QuestActorComponent: '%s'"),
					*ActualItemId.ToString());
			}
		}
		
		if (Condition.ItemId != ActualItemId)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent] ? ItemId mismatch: wanted '%s' got '%s'"),
				*Condition.ItemId.ToString(),
				*ActualItemId.ToString());
			return false; // Item ID mismatch
		}
	}

	// If NpcId is specified, check string param OR QuestActorComponent
	if (!Condition.NpcId.IsNone())
	{
		FName ActualNpcId = Payload.StringParam;
		
		// Try to get NpcId from QuestActorComponent if payload has Target or Instigator
		if (ActualNpcId.IsNone())
		{
			// Try Target first (for dialogue events, Target is usually the NPC)
			if (Payload.TargetActor.IsValid()) // FIXED: use IsValid()
			{
				UQuestActorComponent* QuestActor = Payload.TargetActor->FindComponentByClass<UQuestActorComponent>();
				if (QuestActor)
				{
					ActualNpcId = QuestActor->GetQuestActorId();
					UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent]   ? Got NpcId from TargetActor QuestActorComponent: '%s'"),
						*ActualNpcId.ToString());
				}
			}
			
			// Fallback: Try Instigator
			if (ActualNpcId.IsNone() && Payload.InstigatorActor.IsValid()) // FIXED: use IsValid()
			{
				UQuestActorComponent* QuestActor = Payload.InstigatorActor->FindComponentByClass<UQuestActorComponent>();
				if (QuestActor)
				{
					ActualNpcId = QuestActor->GetQuestActorId();
					UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent]   ? Got NpcId from InstigatorActor QuestActorComponent: '%s'"),
						*ActualNpcId.ToString());
				}
			}
		}
		
		if (Condition.NpcId != ActualNpcId)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent] ? NpcId mismatch: wanted '%s' got '%s'"),
				*Condition.NpcId.ToString(),
				*ActualNpcId.ToString());
			return false; // NPC ID mismatch
		}
	}

	// If RequiredTags specified, check payload tags
	if (Condition.RequiredTags.Num() > 0)
	{
		bool bHasAllTags = Payload.TagsParam.HasAll(Condition.RequiredTags);
		UE_LOG(LogTemp, Verbose, TEXT("[QuestEventBus::DoesConditionMatchEvent] RequiredTags check: %s"), bHasAllTags ? TEXT("PASS") : TEXT("FAIL"));
		
		if (!bHasAllTags)
		{
			return false; // Missing required tags
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestEventBus::DoesConditionMatchEvent] ? ALL CHECKS PASSED - CONDITION MATCHES!"));
	return true; // All checks passed!
}

int32 UQuestEventBus::CalculateProgressAmount(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const
{
	// For most events, use the payload's IntParam (e.g., item count, kill count)
	if (Payload.IntParam > 0)
	{
		return Payload.IntParam;
	}

	// Default: increment by 1
	return 1;
}

void UQuestEventBus::Subscribe(FGameplayTag EventTag, UObject* Owner, FName FunctionName)
{
	if (!Owner || !EventTag.IsValid() || FunctionName.IsNone())
	{
		return;
	}

	// Create subscription
	FQuestEventSubscription Subscription;
	Subscription.EventTag = EventTag;
	Subscription.Owner = Owner;
	Subscription.FunctionName = FunctionName;
	Subscription.bIsActive = true;

	Subscriptions.Add(Subscription);
}

void UQuestEventBus::Unsubscribe(FGameplayTag EventTag, UObject* Owner)
{
	if (!Owner || !EventTag.IsValid())
	{
		return;
	}

	// Remove subscriptions
	Subscriptions.RemoveAll([EventTag, Owner](const FQuestEventSubscription& Sub)
	{
		return Sub.EventTag == EventTag && Sub.Owner == Owner;
	});
}

void UQuestEventBus::UnsubscribeAll(UObject* Owner)
{
	if (!Owner)
	{
		return;
	}

	Subscriptions.RemoveAll([Owner](const FQuestEventSubscription& Sub)
	{
		return Sub.Owner == Owner;
	});
}

bool UQuestEventBus::HasSubscribers(FGameplayTag EventTag) const
{
	for (const FQuestEventSubscription& Sub : Subscriptions)
	{
		if (Sub.EventTag == EventTag && Sub.bIsActive && Sub.Owner.IsValid())
		{
			return true;
		}
	}
	return false;
}

int32 UQuestEventBus::GetSubscriptionCount() const
{
	return Subscriptions.Num();
}

void UQuestEventBus::ClearAllSubscriptions()
{
	Subscriptions.Empty();
	PendingEvents.Empty();
}

void UQuestEventBus::SetEnabled(bool bEnabled)
{
	bIsEnabled = bEnabled;
}

void UQuestEventBus::ProcessPendingEvents()
{
	if (PendingEvents.Num() == 0)
	{
		return;
	}

	// Process all pending events
	TArray<FQuestEventPayload> EventsToProcess = MoveTemp(PendingEvents);
	PendingEvents.Empty();

	for (const FQuestEventPayload& Event : EventsToProcess)
	{
		EmitEvent(Event);
	}
}

bool UQuestEventBus::PassesFilter(const FQuestEventPayload& Payload, const FQuestEventSubscription& Subscription) const
{
	// Check tag filter
	if (Subscription.FilterTags.Num() > 0)
	{
		if (!Payload.TagsParam.HasAny(Subscription.FilterTags))
		{
			return false;
		}
	}

	// Check string param filter
	if (!Subscription.FilterStringParam.IsNone())
	{
		if (Payload.StringParam != Subscription.FilterStringParam)
		{
			return false;
		}
	}

	return true;
}

// Blueprint event emitters

void UQuestEventBus::EmitItemAcquired(FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Item.Acquired"));
	Payload.StringParam = ItemId;
	Payload.IntParam = Count;
	Payload.InstigatorActor = Instigator;
	Payload.TagsParam = Tags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitItemUsed(FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Item.Used"));
	Payload.StringParam = ItemId;
	Payload.InstigatorActor = Instigator;
	Payload.TargetActor = Target;
	Payload.TagsParam = Tags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitNpcTalked(FName NpcId, AActor* Player, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.NPC.Talked"));
	Payload.StringParam = NpcId;
	Payload.InstigatorActor = Player;
	Payload.TagsParam = Tags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitDialogChoice(FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Dialog.Choice"));
	Payload.StringParam = DialogId;
	Payload.TagsParam = Tags;
	Payload.TagsParam.AddTag(FGameplayTag::RequestGameplayTag(FName(*FString::Printf(TEXT("Dialog.Choice.%s"), *ChoiceId.ToString()))));

	EmitEvent(Payload);
}

void UQuestEventBus::EmitEnterArea(FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Area.Enter"));
	Payload.StringParam = AreaId;
	Payload.InstigatorActor = Actor;
	Payload.TagsParam = Tags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitActorKilled(AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Actor.Killed"));
	Payload.InstigatorActor = Killer;
	Payload.TargetActor = Victim;
	Payload.TagsParam = Tags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitTimeReached(int32 Hour, int32 Minute)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Time.Reached"));
	Payload.IntParam = Hour * 60 + Minute;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitDayChanged(int32 DayNumber)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Day.Changed"));
	Payload.IntParam = DayNumber;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitWeatherChanged(const FGameplayTagContainer& WeatherTags)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Weather.Changed"));
	Payload.TagsParam = WeatherTags;

	EmitEvent(Payload);
}

void UQuestEventBus::EmitNeedChanged(FGameplayTag NeedTag, float NewValue, float OldValue)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Need.Changed"));
	Payload.FloatParam = NewValue;
	Payload.TagsParam.AddTag(NeedTag);

	EmitEvent(Payload);
}

void UQuestEventBus::EmitAffectionChanged(FName NpcId, float NewValue, float OldValue)
{
	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Event.Affection.Changed"));
	Payload.StringParam = NpcId;
	Payload.FloatParam = NewValue;

	EmitEvent(Payload);
}
