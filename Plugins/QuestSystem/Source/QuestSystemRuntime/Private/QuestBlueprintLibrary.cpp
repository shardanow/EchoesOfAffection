// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestBlueprintLibrary.h"
#include "QuestSubsystem.h"
#include "QuestEventBus.h"
#include "Engine/World.h"

UQuestSubsystem* UQuestBlueprintLibrary::GetQuestSubsystem(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
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

UQuestEventBus* UQuestBlueprintLibrary::GetEventBus(UObject* WorldContextObject)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetEventBus();
}

// Quest Management

bool UQuestBlueprintLibrary::StartQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	// Use async version with empty callback
	FOnQuestStartedDelegate EmptyCallback;
	Subsystem->StartQuestAsync(QuestId, EmptyCallback);
	return true;  // Returns immediately
}

bool UQuestBlueprintLibrary::CompleteQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->CompleteQuest(QuestId);
}

bool UQuestBlueprintLibrary::FailQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->FailQuest(QuestId);
}

bool UQuestBlueprintLibrary::AbandonQuest(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->AbandonQuest(QuestId);
}

// Quest Queries

EQuestState UQuestBlueprintLibrary::GetQuestState(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return EQuestState::NotStarted;
	}

	return Subsystem->GetQuestState(QuestId);
}

bool UQuestBlueprintLibrary::IsQuestActive(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->IsQuestActive(QuestId);
}

bool UQuestBlueprintLibrary::IsQuestCompleted(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->IsQuestCompleted(QuestId);
}

TArray<FQuestSaveData> UQuestBlueprintLibrary::GetActiveQuests(UObject* WorldContextObject)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return TArray<FQuestSaveData>();
	}

	return Subsystem->GetActiveQuests();
}

UQuestAsset* UQuestBlueprintLibrary::GetQuestAsset(UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetQuestAsset(QuestId);
}

// Objective Management

bool UQuestBlueprintLibrary::UpdateObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, int32 Amount)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->UpdateObjectiveProgress(QuestId, ObjectiveId, Amount);
}

bool UQuestBlueprintLibrary::CompleteObjective(UObject* WorldContextObject, FName QuestId, FName ObjectiveId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->CompleteObjective(QuestId, ObjectiveId);
}

void UQuestBlueprintLibrary::GetObjectiveProgress(UObject* WorldContextObject, FName QuestId, FName ObjectiveId, 
	int32& CurrentCount, int32& TargetCount, bool& bIsCompleted)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem(WorldContextObject);
	if (!Subsystem)
	{
		CurrentCount = 0;
		TargetCount = 1;
		bIsCompleted = false;
		return;
	}

	Subsystem->GetObjectiveProgress(QuestId, ObjectiveId, CurrentCount, TargetCount, bIsCompleted);
}

// Event Emission

void UQuestBlueprintLibrary::EmitItemAcquired(UObject* WorldContextObject, FName ItemId, int32 Count, AActor* Instigator, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitItemAcquired(ItemId, Count, Instigator, Tags);
}

void UQuestBlueprintLibrary::EmitItemUsed(UObject* WorldContextObject, FName ItemId, AActor* Instigator, AActor* Target, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitItemUsed(ItemId, Instigator, Target, Tags);
}

void UQuestBlueprintLibrary::EmitNpcTalked(UObject* WorldContextObject, FName NpcId, AActor* Player, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitNpcTalked(NpcId, Player, Tags);
}

void UQuestBlueprintLibrary::EmitDialogChoice(UObject* WorldContextObject, FName DialogId, FName ChoiceId, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitDialogChoice(DialogId, ChoiceId, Tags);
}

void UQuestBlueprintLibrary::EmitEnterArea(UObject* WorldContextObject, FName AreaId, AActor* Actor, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitEnterArea(AreaId, Actor, Tags);
}

void UQuestBlueprintLibrary::EmitActorKilled(UObject* WorldContextObject, AActor* Victim, AActor* Killer, const FGameplayTagContainer& Tags)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	EventBus->EmitActorKilled(Victim, Killer, Tags);
}

void UQuestBlueprintLibrary::EmitCustomEvent(UObject* WorldContextObject, FGameplayTag EventTag, FName StringParam, int32 IntParam, float FloatParam)
{
	UQuestEventBus* EventBus = GetEventBus(WorldContextObject);
	if (!EventBus)
	{
		return;
	}

	FQuestEventPayload Payload;
	Payload.EventTag = EventTag;
	Payload.StringParam = StringParam;
	Payload.IntParam = IntParam;
	Payload.FloatParam = FloatParam;

	EventBus->EmitEvent(Payload);
}

// Utility

FText UQuestBlueprintLibrary::QuestStateToText(EQuestState State)
{
	switch (State)
	{
	case EQuestState::NotStarted:
		return FText::FromString(TEXT("Not Started"));
	case EQuestState::Active:
		return FText::FromString(TEXT("Active"));
	case EQuestState::Completed:
		return FText::FromString(TEXT("Completed"));
	case EQuestState::Failed:
		return FText::FromString(TEXT("Failed"));
	case EQuestState::Abandoned:
		return FText::FromString(TEXT("Abandoned"));
	default:
		return FText::FromString(TEXT("Unknown"));
	}
}

FText UQuestBlueprintLibrary::ObjectiveStyleToText(EObjectiveStyle Style)
{
	switch (Style)
	{
	case EObjectiveStyle::Collect:
		return FText::FromString(TEXT("Collect"));
	case EObjectiveStyle::Kill:
		return FText::FromString(TEXT("Kill"));
	case EObjectiveStyle::Talk:
		return FText::FromString(TEXT("Talk"));
	case EObjectiveStyle::Visit:
		return FText::FromString(TEXT("Visit"));
	case EObjectiveStyle::Use:
		return FText::FromString(TEXT("Use"));
	case EObjectiveStyle::Deliver:
		return FText::FromString(TEXT("Deliver"));
	case EObjectiveStyle::Craft:
		return FText::FromString(TEXT("Craft"));
	case EObjectiveStyle::Wait:
		return FText::FromString(TEXT("Wait"));
	case EObjectiveStyle::Escort:
		return FText::FromString(TEXT("Escort"));
	case EObjectiveStyle::Discover:
		return FText::FromString(TEXT("Discover"));
	case EObjectiveStyle::Custom:
		return FText::FromString(TEXT("Custom"));
	default:
		return FText::FromString(TEXT("Unknown"));
	}
}

FLinearColor UQuestBlueprintLibrary::QuestStateToColor(EQuestState State)
{
	switch (State)
	{
	case EQuestState::NotStarted:
		return FLinearColor::Gray;
	case EQuestState::Active:
		return FLinearColor::Yellow;
	case EQuestState::Completed:
		return FLinearColor::Green;
	case EQuestState::Failed:
		return FLinearColor::Red;
	case EQuestState::Abandoned:
		return FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
	default:
		return FLinearColor::White;
	}
}

