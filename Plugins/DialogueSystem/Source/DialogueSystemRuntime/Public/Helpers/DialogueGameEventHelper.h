// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Forward declarations
class UDialogueSubsystem;
class AActor;
struct FGameEventPayload;

/**
 * Helper class for emitting dialogue events to GameEventBus
 * Provides soft dependency integration without hard coupling
 */
class FDialogueGameEventHelper
{
public:
	/** Emit dialogue started event */
	static void EmitDialogueStartedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, FName DialogueId);

	/** Emit dialogue ended event */
	static void EmitDialogueEndedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC);

	/** Emit node changed event */
	static void EmitNodeChangedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, FName NodeId);

	/** Emit choice selected event */
	static void EmitChoiceSelectedEvent(UDialogueSubsystem* Subsystem, AActor* Player, AActor* NPC, int32 ChoiceIndex);

private:
	/** Check if GameEventBus is available */
	static bool IsGameEventBusAvailable();

	/** Send event to GameEventBus */
	static void SendGameEvent(UObject* WorldContextObject, FGameplayTag EventTag, AActor* Instigator, AActor* Target, const TMap<FName, FString>& Payload);
};
