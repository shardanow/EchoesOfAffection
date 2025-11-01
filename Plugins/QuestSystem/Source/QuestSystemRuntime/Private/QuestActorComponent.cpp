// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestActorComponent.h"
#include "GameplayTagAssetInterface.h"
#include "GameEventBusSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogQuestActorComponent, Log, All);

UQuestActorComponent::UQuestActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Default settings
	bAutoAssignId = true;
	bCanGiveQuests = false;
	bCanBeQuestTarget = true;
	bCanEmitQuestEvents = true;
	bCanBeTracked = true;
	
	bWasIdAutoAssigned = false;
}

void UQuestActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Auto-assign ID if needed
	if (bAutoAssignId && QuestActorId.IsNone())
	{
		AutoAssignQuestActorId();
	}
	
	// Cache final ID
	CachedQuestActorId = QuestActorId;
	
	// Validate configuration
	ValidateSettings();
	
	// Log initialization
	AActor* Owner = GetOwner();
	UE_LOG(LogQuestActorComponent, Log, TEXT("QuestActorComponent initialized on '%s' with ID '%s'%s"),
		Owner ? *Owner->GetName() : TEXT("None"),
		*CachedQuestActorId.ToString(),
		bWasIdAutoAssigned ? TEXT(" (auto-assigned)") : TEXT(""));
	
	// Log roles
	if (bCanGiveQuests || bCanBeQuestTarget || bCanEmitQuestEvents)
	{
		TArray<FString> Roles;
		if (bCanGiveQuests) Roles.Add(TEXT("QuestGiver"));
		if (bCanBeQuestTarget) Roles.Add(TEXT("Target"));
		if (bCanEmitQuestEvents) Roles.Add(TEXT("EventEmitter"));
		if (bCanBeTracked) Roles.Add(TEXT("Trackable"));
		
		UE_LOG(LogQuestActorComponent, Log, TEXT("  Roles: %s"), *FString::Join(Roles, TEXT(", ")));
	}
	
	// Log tags if any
	if (QuestActorTags.Num() > 0)
	{
		UE_LOG(LogQuestActorComponent, Log, TEXT("  Quest Tags: %s"), *QuestActorTags.ToStringSimple());
	}
}

#if WITH_EDITOR
void UQuestActorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	
	// If bAutoAssignId changed to true, try to auto-assign
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UQuestActorComponent, bAutoAssignId))
	{
		if (bAutoAssignId && QuestActorId.IsNone())
		{
			AutoAssignQuestActorId();
		}
	}
	
	// Validate on any change
	ValidateSettings();
}
#endif

// === CORE METHODS ===

void UQuestActorComponent::AutoAssignQuestActorId()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogQuestActorComponent, Warning, TEXT("Cannot auto-assign ID: Owner is null"));
		return;
	}
	
	// 1. Try to extract from GameplayTags
	FName IdFromTags = ExtractIdFromGameplayTags();
	if (!IdFromTags.IsNone())
	{
		QuestActorId = IdFromTags;
		bWasIdAutoAssigned = true;
		UE_LOG(LogQuestActorComponent, Log, TEXT("Auto-assigned QuestActorId from GameplayTags: '%s' -> '%s'"),
			*Owner->GetName(), *QuestActorId.ToString());
		return;
	}
	
	// 2. Fallback: extract from actor name
	FName IdFromName = ExtractIdFromActorName();
	if (!IdFromName.IsNone())
	{
		QuestActorId = IdFromName;
		bWasIdAutoAssigned = true;
		UE_LOG(LogQuestActorComponent, Log, TEXT("Auto-assigned QuestActorId from actor name: '%s' -> '%s'"),
			*Owner->GetName(), *QuestActorId.ToString());
		return;
	}
	
	// 3. Last resort: use actor name as-is
	QuestActorId = FName(*Owner->GetName());
	bWasIdAutoAssigned = true;
	UE_LOG(LogQuestActorComponent, Warning, TEXT("Auto-assigned QuestActorId as-is (no simplification): '%s'"),
		*QuestActorId.ToString());
}

FName UQuestActorComponent::ExtractIdFromGameplayTags() const
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetClass()->ImplementsInterface(UGameplayTagAssetInterface::StaticClass()))
	{
		return NAME_None;
	}
	
	IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Owner);
	if (!TagInterface)
	{
		return NAME_None;
	}
	
	FGameplayTagContainer Tags;
	TagInterface->GetOwnedGameplayTags(Tags);
	
	// Look for NPC.* tag (primary use case)
	FGameplayTag NPCParent = FGameplayTag::RequestGameplayTag(FName("NPC"), false);
	if (NPCParent.IsValid())
	{
		for (const FGameplayTag& Tag : Tags)
		{
			if (Tag.MatchesTag(NPCParent))
			{
				// Extract last part of tag (e.g., "NPC.Lianne" -> "Lianne")
				FString TagString = Tag.ToString();
				int32 LastDotIndex;
				if (TagString.FindLastChar('.', LastDotIndex))
				{
					FName ExtractedId = FName(*TagString.RightChop(LastDotIndex + 1));
					UE_LOG(LogQuestActorComponent, Verbose, TEXT("Extracted ID from GameplayTag '%s': '%s'"),
						*TagString, *ExtractedId.ToString());
					return ExtractedId;
				}
			}
		}
	}
	
	// Look for Quest.Actor.* tag (alternative)
	FGameplayTag QuestActorParent = FGameplayTag::RequestGameplayTag(FName("Quest.Actor"), false);
	if (QuestActorParent.IsValid())
	{
		for (const FGameplayTag& Tag : Tags)
		{
			if (Tag.MatchesTag(QuestActorParent))
			{
				// Extract last part of tag
				FString TagString = Tag.ToString();
				int32 LastDotIndex;
				if (TagString.FindLastChar('.', LastDotIndex))
				{
					FName ExtractedId = FName(*TagString.RightChop(LastDotIndex + 1));
					UE_LOG(LogQuestActorComponent, Verbose, TEXT("Extracted ID from Quest.Actor tag '%s': '%s'"),
						*TagString, *ExtractedId.ToString());
					return ExtractedId;
				}
			}
		}
	}
	
	return NAME_None;
}

FName UQuestActorComponent::ExtractIdFromActorName() const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return NAME_None;
	}
	
	FString ActorName = Owner->GetName();
	
	// Remove common prefixes
	ActorName.RemoveFromStart(TEXT("BP_"));
	ActorName.RemoveFromStart(TEXT("SM_"));
	ActorName.RemoveFromStart(TEXT("SK_"));
	ActorName.RemoveFromStart(TEXT("NPC_"));
	
	// Take part before first underscore
	int32 UnderscoreIndex;
	if (ActorName.FindChar('_', UnderscoreIndex))
	{
		ActorName = ActorName.Left(UnderscoreIndex);
	}
	
	if (ActorName.IsEmpty())
	{
		return NAME_None;
	}
	
	return FName(*ActorName);
}

void UQuestActorComponent::ValidateSettings()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	
	// Warn if ID is still empty
	if (QuestActorId.IsNone())
	{
		UE_LOG(LogQuestActorComponent, Warning, TEXT("QuestActorId is None on '%s'. Set manually or enable bAutoAssignId."),
			*Owner->GetName());
	}
	
	// Warn if bCanGiveQuests but no QuestGiverComponent
	if (bCanGiveQuests)
	{
		UActorComponent* QuestGiver = Owner->GetComponentByClass(UActorComponent::StaticClass());
		bool bHasQuestGiver = false;
		
		// Simple check - in real implementation would check for UQuestGiverComponent
		TArray<UActorComponent*> Components;
		Owner->GetComponents(Components);
		for (UActorComponent* Component : Components)
		{
			if (Component && Component->GetClass()->GetName().Contains(TEXT("QuestGiver")))
			{
				bHasQuestGiver = true;
				break;
			}
		}
		
		if (!bHasQuestGiver)
		{
			UE_LOG(LogQuestActorComponent, Warning, TEXT("bCanGiveQuests is true but no QuestGiverComponent found on '%s'"),
				*Owner->GetName());
		}
	}
	
	// Warn if DisplayName is empty
	if (DisplayName.IsEmpty())
	{
		UE_LOG(LogQuestActorComponent, Verbose, TEXT("DisplayName is empty on '%s'. Consider setting for better UI."),
			*Owner->GetName());
	}
	
	// Info if no roles are enabled
	if (!bCanGiveQuests && !bCanBeQuestTarget && !bCanEmitQuestEvents)
	{
		UE_LOG(LogQuestActorComponent, Warning, TEXT("No quest roles enabled on '%s'. Component will have limited functionality."),
			*Owner->GetName());
	}
}

// === API IMPLEMENTATION ===

FName UQuestActorComponent::GetQuestActorId() const
{
	// Return cached ID if available, otherwise current ID
	return CachedQuestActorId.IsNone() ? QuestActorId : CachedQuestActorId;
}

FText UQuestActorComponent::GetDisplayNameOrId() const
{
	if (!DisplayName.IsEmpty())
	{
		return DisplayName;
	}
	
	FName Id = GetQuestActorId();
	return FText::FromName(Id);
}

bool UQuestActorComponent::HasQuestTag(FGameplayTag Tag) const
{
	return QuestActorTags.HasTag(Tag);
}

bool UQuestActorComponent::HasQuestTagMatching(FGameplayTag ParentTag) const
{
	return QuestActorTags.HasTag(ParentTag);
}

FString UQuestActorComponent::GetMetadata(FName Key, bool& bFound) const
{
	const FString* Value = Metadata.Find(Key);
	bFound = (Value != nullptr);
	return bFound ? *Value : FString();
}

void UQuestActorComponent::SetMetadata(FName Key, const FString& Value)
{
	Metadata.Add(Key, Value);
	UE_LOG(LogQuestActorComponent, Verbose, TEXT("Set metadata '%s' = '%s' on '%s'"),
		*Key.ToString(), *Value, *GetQuestActorId().ToString());
}

void UQuestActorComponent::RemoveMetadata(FName Key)
{
	if (Metadata.Remove(Key) > 0)
	{
		UE_LOG(LogQuestActorComponent, Verbose, TEXT("Removed metadata '%s' from '%s'"),
			*Key.ToString(), *GetQuestActorId().ToString());
	}
}

bool UQuestActorComponent::HasMetadata(FName Key) const
{
	return Metadata.Contains(Key);
}

TArray<FName> UQuestActorComponent::GetMetadataKeys() const
{
	TArray<FName> Keys;
	Metadata.GetKeys(Keys);
	return Keys;
}

void UQuestActorComponent::EmitQuestEvent(FGameplayTag EventTag, int32 IntParam, AActor* Target)
{
	if (!bCanEmitQuestEvents)
	{
		UE_LOG(LogQuestActorComponent, Warning, TEXT("Cannot emit quest event: bCanEmitQuestEvents is false on '%s'"),
			*GetQuestActorId().ToString());
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogQuestActorComponent, Error, TEXT("Cannot emit quest event: World is null"));
		return;
	}
	
	UGameEventBusSubsystem* EventBus = UGameEventBusSubsystem::Get(World);
	if (!EventBus)
	{
		UE_LOG(LogQuestActorComponent, Error, TEXT("Cannot emit quest event: GameEventBusSubsystem not found"));
		return;
	}
	
	// Emit event with this actor as instigator
	FGameEventPayload Payload;
	Payload.EventTag = EventTag;
	Payload.StringParam = GetQuestActorId(); // Use quest actor ID
	Payload.IntParam = IntParam;
	Payload.InstigatorActor = GetOwner(); // FIXED: was 'Instigator'
	Payload.TargetActor = Target; // FIXED: was 'Target'
	
	EventBus->EmitEvent(Payload);
	
	UE_LOG(LogQuestActorComponent, Log, TEXT("Emitted quest event '%s' from '%s' (IntParam=%d)"),
		*EventTag.ToString(), *GetQuestActorId().ToString(), IntParam);
}

void UQuestActorComponent::ValidateConfiguration()
{
	ValidateSettings();
}

bool UQuestActorComponent::IsValidConfiguration() const
{
	// Basic validation
	if (QuestActorId.IsNone())
	{
		return false;
	}
	
	// At least one role should be enabled
	if (!bCanGiveQuests && !bCanBeQuestTarget && !bCanEmitQuestEvents)
	{
		return false;
	}
	
	return true;
}
