// Copyright Epic Games, Inc. All Rights Reserved.

#include "Helpers/DialogueGameEventHelper.h"
#include "Subsystems/DialogueSubsystem.h"
#include "Engine/World.h"

// Conditional include for GameEventBus
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

void FDialogueGameEventHelper::EmitDialogueStartedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, FName DialogueId)
{
	if (!Subsystem)
	{
		return;
	}

	TMap<FName, FString> Payload;
	Payload.Add("DialogueId", DialogueId.ToString());

	SendGameEvent(
		Subsystem,
		FGameplayTag::RequestGameplayTag("Dialogue.Event.Started", false),
		Player,
		NPC,
		Payload
	);
}

void FDialogueGameEventHelper::EmitDialogueEndedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC)
{
	if (!Subsystem)
	{
		return;
	}

	SendGameEvent(
		Subsystem,
		FGameplayTag::RequestGameplayTag("Dialogue.Event.Ended", false),
		Player,
		NPC,
		TMap<FName, FString>()
	);
}

void FDialogueGameEventHelper::EmitNodeChangedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, FName NodeId)
{
	if (!Subsystem)
	{
		return;
	}

	TMap<FName, FString> Payload;
	Payload.Add("NodeId", NodeId.ToString());

	SendGameEvent(
		Subsystem,
		FGameplayTag::RequestGameplayTag("Dialogue.Event.NodeChanged", false),
		Player,
		NPC,
		Payload
	);
}

void FDialogueGameEventHelper::EmitChoiceSelectedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, int32 ChoiceIndex)
{
	if (!Subsystem)
	{
		return;
	}

	TMap<FName, FString> Payload;
	Payload.Add("ChoiceIndex", FString::FromInt(ChoiceIndex));

	SendGameEvent(
		Subsystem,
		FGameplayTag::RequestGameplayTag("Dialogue.Event.ChoiceSelected", false),
		Player,
		NPC,
		Payload
	);
}

bool FDialogueGameEventHelper::IsGameEventBusAvailable()
{
#if GAMEEVENTBUS_AVAILABLE
	return FModuleManager::Get().IsModuleLoaded("GameEventBus");
#else
	return false;
#endif
}

void FDialogueGameEventHelper::SendGameEvent(UObject* WorldContextObject, FGameplayTag EventTag, AActor* Instigator, AActor* Target, const TMap<FName, FString>& Payload)
{
#if GAMEEVENTBUS_AVAILABLE
	if (!IsGameEventBusAvailable() || !EventTag.IsValid())
	{
		return;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UGameEventBusSubsystem* EventBus = GameInstance->GetSubsystem<UGameEventBusSubsystem>();
	if (!EventBus)
	{
		return;
	}

	// Create payload
	FGameEventPayload EventPayload(EventTag);
	EventPayload.InstigatorActor = Instigator;
	EventPayload.TargetActor = Target;

	// Add payload parameters
	if (const FString* DialogueId = Payload.Find("DialogueId"))
	{
		EventPayload.StringParam = FName(**DialogueId);
	}
	else if (const FString* NodeId = Payload.Find("NodeId"))
	{
		EventPayload.StringParam = FName(**NodeId);
	}
	else if (const FString* ChoiceIndex = Payload.Find("ChoiceIndex"))
	{
		EventPayload.IntParam = FCString::Atoi(**ChoiceIndex);
	}

	// Emit event
	EventBus->EmitEvent(EventPayload);

	UE_LOG(LogTemp, Verbose, TEXT("?? DialogueGameEventHelper: Emitted %s"), *EventTag.ToString());
#endif
}
