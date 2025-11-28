// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/DialogueEffect_PositionParticipant.h"
#include "Effects/DialogueEffect_PositionAIMoveTo.h"
#include "Effects/DialogueEffect_PositionTeleport.h"
#include "Effects/DialogueEffect_PositionConditionalTeleport.h"
#include "Core/DialogueContext.h" // Correct path for DialogueSessionContext
#include "Core/DialogueRunner.h" // For accessing LoadedDialogue
#include "Data/DialogueDataAsset.h" // For DialoguePersonaData
#include "AIController.h"
#include "EngineUtils.h"
#include "GameplayTagsManager.h"

// Forward declare to avoid circular dependency (DialogueLocationMarker is in Runtime module)
class ADialogueLocationMarker;

// Include from Runtime module (only in .cpp to avoid circular dependency)
#include "Actors/DialogueLocationMarker.h"
#include "Subsystems/DialogueLocationSubsystem.h"

// NEW v1.17.1: GameEventBus integration for camera synchronization
#if WITH_GAMEEVENTBUS
#include "GameEventBusSubsystem.h"
#endif

DEFINE_LOG_CATEGORY(LogDialoguePositioning);

UDialogueEffect_PositionParticipant::UDialogueEffect_PositionParticipant()
{
	bSupportsReverse = false; // Positioning is one-way operation
}

void UDialogueEffect_PositionParticipant::Execute_Implementation(UDialogueSessionContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("Execute: Invalid context"));
		return;
	}

	// Find participant actor
	AActor* ParticipantActor = FindParticipantActor(Context, PositioningConfig.ParticipantId);
	if (!ParticipantActor)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("Execute: Failed to find participant '%s'"), *PositioningConfig.ParticipantId.ToString());
		return;
	}

	// Resolve target transform
	FTransform TargetTransform;
	FString ErrorMessage;
	if (!ResolveTargetTransform(Context, TargetTransform, ErrorMessage))
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("Execute: Failed to resolve target transform for '%s': %s"), 
			*PositioningConfig.ParticipantId.ToString(), *ErrorMessage);
		return;
	}

	// Log positioning info
	LogPositioningInfo(ParticipantActor, TargetTransform);

	// Execute specific positioning implementation
	ExecutePositioning(ParticipantActor, TargetTransform, Context);

	// NEW v1.17.1: Emit event AFTER positioning completes
	EmitParticipantPositionedEvent(Context, PositioningConfig.ParticipantId);
}

FText UDialogueEffect_PositionParticipant::GetDisplayText_Implementation() const
{
	FString ModeString;
	switch (PositioningConfig.PositioningMode)
	{
		case EDialoguePositioningMode::AIMoveTo:
			ModeString = TEXT("Move");
			break;
		case EDialoguePositioningMode::Teleport:
			ModeString = TEXT("Teleport");
			break;
		case EDialoguePositioningMode::ConditionalTeleport:
			ModeString = TEXT("Smart Move");
			break;
	}

	FString LocationString;
	switch (PositioningConfig.LocationSource)
	{
		case EDialogueLocationSource::WorldCoordinates:
			LocationString = FString::Printf(TEXT("(%s)"), *PositioningConfig.TargetLocation.ToString());
			break;
		case EDialogueLocationSource::MarkerTag:
			LocationString = PositioningConfig.MarkerTag.ToString();
			break;
		case EDialogueLocationSource::ActorTransform:
			LocationString = TEXT("Actor Transform");
			break;
	}

	return FText::FromString(FString::Printf(TEXT("%s: %s to %s"), 
		*ModeString, *PositioningConfig.ParticipantId.ToString(), *LocationString));
}

bool UDialogueEffect_PositionParticipant::ResolveTargetTransform(
	UDialogueSessionContext* Context,
	FTransform& OutTransform,
	FString& OutErrorMessage) const
{
	switch (PositioningConfig.LocationSource)
	{
		case EDialogueLocationSource::WorldCoordinates:
		{
			OutTransform.SetLocation(PositioningConfig.TargetLocation);
			OutTransform.SetRotation(PositioningConfig.TargetRotation.Quaternion());
			OutTransform.SetScale3D(FVector::OneVector);
			return true;
		}

		case EDialogueLocationSource::MarkerTag:
		{
			if (!PositioningConfig.MarkerTag.IsValid())
			{
				OutErrorMessage = TEXT("MarkerTag is not valid");
				return false;
			}

			UWorld* World = Context ? Context->GetWorld() : nullptr;
			if (!World)
			{
				OutErrorMessage = TEXT("Invalid world");
				return false;
			}

			ADialogueLocationMarker* Marker = FindLocationMarkerByTag(World, PositioningConfig.MarkerTag);
			if (!Marker)
			{
				OutErrorMessage = FString::Printf(TEXT("Location marker with tag '%s' not found"), 
					*PositioningConfig.MarkerTag.ToString());
				return false;
			}

			OutTransform = Marker->GetTargetTransform();
			return true;
		}

		case EDialogueLocationSource::ActorTransform:
		{
			AActor* TargetActor = PositioningConfig.TargetActor.Get();
			if (!TargetActor)
			{
				OutErrorMessage = TEXT("Target actor is null or not loaded");
				return false;
			}

			OutTransform = TargetActor->GetActorTransform();
			return true;
		}

		default:
		{
			OutErrorMessage = TEXT("Unknown LocationSource type");
			return false;
		}
	}
}

AActor* UDialogueEffect_PositionParticipant::FindParticipantActor(
	UDialogueSessionContext* Context,
	FName ParticipantId) const
{
	if (!Context)
	{
		return nullptr;
	}

	// NEW v1.16: Use PersonaId mapping from Participants component
	UDialogueParticipants* Participants = Context->GetParticipants();
	if (!Participants)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("FindParticipantActor: No Participants component in context"));
		return nullptr;
	}

	// Try to find actor by PersonaId
	AActor* Actor = Participants->GetActorByPersonaId(ParticipantId);
	if (Actor)
	{
		UE_LOG(LogDialoguePositioning, Log, TEXT("FindParticipantActor: Found actor '%s' for Persona '%s'"), 
			*Actor->GetName(), *ParticipantId.ToString());
		return Actor;
	}

	// Not found - log available personas for debugging
	UE_LOG(LogDialoguePositioning, Warning, TEXT("FindParticipantActor: Participant '%s' not found in dialogue"), 
		*ParticipantId.ToString());
	
	TArray<FName> AvailablePersonas = Participants->GetAllPersonaIds();
	if (AvailablePersonas.Num() > 0)
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("  Available Personas: %s"), 
			*FString::JoinBy(AvailablePersonas, TEXT(", "), [](const FName& Id) { return Id.ToString(); }));

		// Try to suggest closest match (if any)
		FName ClosestMatch = NAME_None;
		int32 ClosestDistance = TNumericLimits<int32>::Max();
		for (FName PersonaId : AvailablePersonas)
		{
			int32 Distance = FCString::Strlen(*ParticipantId.ToString()) - FCString::Strlen(*PersonaId.ToString());
			if (Distance < ClosestDistance)
			{
				ClosestDistance = Distance;
				ClosestMatch = PersonaId;
			}
		}

		if (ClosestMatch != NAME_None)
		{
			UE_LOG(LogDialoguePositioning, Warning, TEXT("  Did you mean '%s'? (PersonaId similarity)"), 
				*ClosestMatch.ToString());
		}
	}
	else
	{
		UE_LOG(LogDialoguePositioning, Warning, TEXT("  No personas registered in dialogue (check DialogueRunner setup)"));
	}

	return nullptr;
}

ADialogueLocationMarker* UDialogueEffect_PositionParticipant::FindLocationMarkerByTag(
	UWorld* World,
	FGameplayTag MarkerTag) const
{
	if (!World || !MarkerTag.IsValid())
	{
		return nullptr;
	}

	// Try using subsystem first (cached lookup)
	if (UDialogueLocationSubsystem* LocationSubsystem = World->GetSubsystem<UDialogueLocationSubsystem>())
	{
		ADialogueLocationMarker* Marker = LocationSubsystem->FindMarkerByTag(MarkerTag);
		if (Marker)
		{
			UE_LOG(LogDialoguePositioning, Log, TEXT("FindLocationMarkerByTag: Found marker '%s' via subsystem at %s"), 
				*MarkerTag.ToString(), *Marker->GetActorLocation().ToString());
			return Marker;
		}
	}

	// Fallback: direct iteration (if subsystem not available)
	UE_LOG(LogDialoguePositioning, Warning, TEXT("FindLocationMarkerByTag: Subsystem not available, using direct search"));

	for (TActorIterator<ADialogueLocationMarker> It(World); It; ++It)
	{
		ADialogueLocationMarker* Marker = *It;
		if (Marker && Marker->LocationTag == MarkerTag)
		{
			UE_LOG(LogDialoguePositioning, Log, TEXT("FindLocationMarkerByTag: Found marker '%s' at %s"), 
				*MarkerTag.ToString(), *Marker->GetActorLocation().ToString());
			return Marker;
		}
	}

	UE_LOG(LogDialoguePositioning, Warning, TEXT("FindLocationMarkerByTag: No marker found with tag '%s'"), 
		*MarkerTag.ToString());
	return nullptr;
}

AAIController* UDialogueEffect_PositionParticipant::GetAIController(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	// Try casting to Pawn first
	APawn* Pawn = Cast<APawn>(Actor);
	if (!Pawn)
	{
		return nullptr;
	}

	// Get AI controller
	AAIController* AIController = Cast<AAIController>(Pawn->GetController());
	return AIController;
}

void UDialogueEffect_PositionParticipant::ExecutePositioning(
	AActor* ParticipantActor,
	const FTransform& TargetTransform,
	UDialogueSessionContext* Context)
{
	// Base implementation does nothing - override in derived classes
	UE_LOG(LogDialoguePositioning, Warning, TEXT("ExecutePositioning: Base class called - override in derived class"));
}

UDialogueEffect_PositionParticipant* UDialogueEffect_PositionParticipant::CreatePositioningEffect(
	UObject* Outer,
	const FDialogueParticipantPositioning& Config)
{
	return UDialoguePositioningEffectFactory::CreateEffect(Outer, Config);
}

void UDialogueEffect_PositionParticipant::LogPositioningInfo(
	AActor* ParticipantActor,
	const FTransform& TargetTransform,
	const FString& AdditionalInfo) const
{
	UE_LOG(LogDialoguePositioning, Log, TEXT("Positioning '%s':"), *PositioningConfig.ParticipantId.ToString());
	UE_LOG(LogDialoguePositioning, Log, TEXT("  Mode: %d"), static_cast<int32>(PositioningConfig.PositioningMode));
	UE_LOG(LogDialoguePositioning, Log, TEXT("  From: %s"), *ParticipantActor->GetActorLocation().ToString());
	UE_LOG(LogDialoguePositioning, Log, TEXT("  To: %s"), *TargetTransform.GetLocation().ToString());
	UE_LOG(LogDialoguePositioning, Log, TEXT("  Distance: %.2f"), 
		FVector::Dist(ParticipantActor->GetActorLocation(), TargetTransform.GetLocation()));
	
	if (!AdditionalInfo.IsEmpty())
	{
		UE_LOG(LogDialoguePositioning, Log, TEXT("  Info: %s"), *AdditionalInfo);
	}
}

//==============================================================================
// NEW v1.17.1: GameEventBus Integration - Participant Positioned Event
//==============================================================================

void UDialogueEffect_PositionParticipant::EmitParticipantPositionedEvent(UDialogueSessionContext* Context, FName ParticipantId)
{
#if WITH_GAMEEVENTBUS
    if (!Context)
    {
        return;
    }

    UWorld* World = Context->GetWorld();
    if (!World)
    {
        return;
    }

    UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
    if (!EventBus)
  {
        return;
  }

    // Get participant actor
    UDialogueParticipants* Participants = Context->GetParticipants();
    AActor* ParticipantActor = Participants ? Participants->GetActorByPersonaId(ParticipantId) : nullptr;

    // Prepare payload
    FGameEventPayload Payload;
    Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Dialogue.ParticipantPositioned"));
 Payload.StringParam = ParticipantId;
    Payload.TargetActor = ParticipantActor;

    // Emit event for camera to listen
    EventBus->EmitEvent(Payload, false);

    UE_LOG(LogDialoguePositioning, Log, TEXT("[GAMEEVENTBUS] Emitted Dialogue.ParticipantPositioned for '%s'"), 
        *ParticipantId.ToString());

#endif // WITH_GAMEEVENTBUS
}

//==============================================================================
// Factory Implementation
//==============================================================================

UDialogueEffect_PositionParticipant* UDialoguePositioningEffectFactory::CreateEffect(
	UObject* Outer,
	const FDialogueParticipantPositioning& Config)
{
	if (!Outer)
	{
		UE_LOG(LogDialoguePositioning, Error, TEXT("CreateEffect: Invalid Outer object"));
		return nullptr;
	}

	UDialogueEffect_PositionParticipant* Effect = nullptr;

	// Create appropriate effect based on positioning mode
	switch (Config.PositioningMode)
	{
		case EDialoguePositioningMode::AIMoveTo:
		{
			UDialogueEffect_PositionAIMoveTo* AIEffect = NewObject<UDialogueEffect_PositionAIMoveTo>(Outer);
			AIEffect->PositioningConfig = Config;
			Effect = AIEffect;
			UE_LOG(LogDialoguePositioning, Log, TEXT("CreateEffect: Created AIMoveTo effect for '%s'"), *Config.ParticipantId.ToString());
			break;
		}

		case EDialoguePositioningMode::Teleport:
		{
			UDialogueEffect_PositionTeleport* TeleportEffect = NewObject<UDialogueEffect_PositionTeleport>(Outer);
			TeleportEffect->PositioningConfig = Config;
			Effect = TeleportEffect;
			UE_LOG(LogDialoguePositioning, Log, TEXT("CreateEffect: Created Teleport effect for '%s'"), *Config.ParticipantId.ToString());
			break;
		}

		case EDialoguePositioningMode::ConditionalTeleport:
		{
			UDialogueEffect_PositionConditionalTeleport* ConditionalEffect = NewObject<UDialogueEffect_PositionConditionalTeleport>(Outer);
			ConditionalEffect->PositioningConfig = Config;
			Effect = ConditionalEffect;
			UE_LOG(LogDialoguePositioning, Log, TEXT("CreateEffect: Created ConditionalTeleport effect for '%s'"), *Config.ParticipantId.ToString());
			break;
		}

		default:
		{
			UE_LOG(LogDialoguePositioning, Error, TEXT("CreateEffect: Unknown positioning mode %d"), static_cast<int32>(Config.PositioningMode));
			return nullptr;
		}
	}

	return Effect;
}

TArray<UDialogueEffect_PositionParticipant*> UDialoguePositioningEffectFactory::CreateEffectsForNode(
	UObject* Outer,
	const FDialogueNodePositioning& NodePositioning,
	FName PrimaryPersonaId)
{
	TArray<UDialogueEffect_PositionParticipant*> Effects;

	if (!NodePositioning.bEnablePositioning)
	{
		return Effects;
	}

	// Create effect for Primary Persona if configured
	if (NodePositioning.PrimaryPersonaPositioning.ParticipantId != NAME_None)
	{
		UDialogueEffect_PositionParticipant* Effect = CreateEffect(Outer, NodePositioning.PrimaryPersonaPositioning);
		if (Effect)
		{
			Effects.Add(Effect);
		}
	}

	// Create effects for Additional Personas
	for (const auto& Pair : NodePositioning.AdditionalPersonasPositioning)
	{
		UDialogueEffect_PositionParticipant* Effect = CreateEffect(Outer, Pair.Value);
		if (Effect)
		{
			Effects.Add(Effect);
		}
	}

	UE_LOG(LogDialoguePositioning, Log, TEXT("CreateEffectsForNode: Created %d positioning effects"), Effects.Num());
	return Effects;
}
