// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_EmitGameEvent.h"
#include "Core/DialogueContext.h"

// Conditional include - only if GameEventBus is available
#if __has_include("GameEventBusSubsystem.h")
	#include "GameEventBusSubsystem.h"
	#include "GameEventBusLibrary.h"
	#define GAMEEVENTBUS_AVAILABLE 1
#else
	#define GAMEEVENTBUS_AVAILABLE 0
#endif

DEFINE_LOG_CATEGORY_STATIC(LogDialogueGameEvents, Log, All);

void UDialogueEffect_EmitGameEvent::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!EventTag.IsValid())
	{
		UE_LOG(LogDialogueGameEvents, Warning, TEXT("EmitGameEvent: Invalid EventTag!"));
		return;
	}

#if GAMEEVENTBUS_AVAILABLE
	// Get GameEventBus
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(Context);
	if (!EventBus)
	{
		UE_LOG(LogDialogueGameEvents, Warning, TEXT("EmitGameEvent: GameEventBusSubsystem not found!"));
		return;
	}

	// Build payload
	FGameEventPayload Payload;
	Payload.EventTag = EventTag;
	Payload.StringParam = StringParam;
	Payload.StringParam2 = StringParam2;
	Payload.IntParam = IntParam;
	Payload.FloatParam = FloatParam;
	Payload.AdditionalTags = AdditionalTags;

	// Set actors from dialogue context
	if (Context)
	{
		if (bUsePlayerAsInstigator)
		{
			AActor* PlayerActor = Context->GetPlayer();
			if (PlayerActor)
			{
				Payload.InstigatorActor = PlayerActor;
			}
		}

		if (bUseNPCAsTarget)
		{
			AActor* NPCActor = Context->GetNPC();
			if (NPCActor)
			{
				Payload.TargetActor = NPCActor;
			}
		}

		// Store context as payload object
		Payload.PayloadObject = Context;
	}

	// Emit event
	EventBus->EmitEvent(Payload, bLogEvent);

	if (bLogEvent)
	{
		UE_LOG(LogDialogueGameEvents, Log, TEXT("Emitted game event from dialogue: %s (String: %s, Int: %d)"), 
			*EventTag.ToString(), 
			*StringParam.ToString(), 
			IntParam);
	}
#else
	UE_LOG(LogDialogueGameEvents, Error, TEXT("EmitGameEvent: GameEventBus plugin not enabled! Cannot emit event."));
#endif
}

FText UDialogueEffect_EmitGameEvent::GetDisplayText_Implementation() const
{
	if (StringParam.IsNone())
	{
		return FText::FromString(FString::Printf(TEXT("Emit Event: %s"), *EventTag.ToString()));
	}
	else
	{
		return FText::FromString(FString::Printf(TEXT("Emit Event: %s (%s)"), 
			*EventTag.ToString(), 
			*StringParam.ToString()));
	}
}

void UDialogueEffect_EmitMultipleGameEvents::Execute_Implementation(UDialogueSessionContext* Context)
{
	for (UDialogueEffect_EmitGameEvent* EventEffect : EventsToEmit)
	{
		if (EventEffect)
		{
			EventEffect->Execute(Context);
		}
	}
}

FText UDialogueEffect_EmitMultipleGameEvents::GetDisplayText_Implementation() const
{
	return FText::FromString(FString::Printf(TEXT("Emit %d Events"), EventsToEmit.Num()));
}
