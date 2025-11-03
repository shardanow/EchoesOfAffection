// Copyright Epic Games, Inc. All Rights Reserved.

#include "SaveGame/RelationshipSaveGame.h"
#include "Subsystems/RelationshipSubsystem.h"
#include "Components/NPCRelationshipComponent.h"
#include "RelationshipSystemCore.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

const FString URelationshipSaveGameHelper::DefaultSlotName = TEXT("RelationshipSave");

// URelationshipSaveGame implementation

URelationshipSaveGame::URelationshipSaveGame()
	: SaveVersion(CURRENT_VERSION)
	, SavedGameTime(0.0f)
{
}

void URelationshipSaveGame::SerializeFromSubsystem(URelationshipSubsystem* Subsystem)
{
	if (!Subsystem)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Cannot serialize: Subsystem is null"));
		return;
	}

	SerializedRelationships.Empty();

	// Get all relationships from subsystem via query methods
	// Since Relationships map is private, we need to iterate through registered components
	UWorld* World = Subsystem->GetWorld();
	if (!World)
	{
		return;
	}

	// Collect all actors with relationship components
	TArray<AActor*> AllActorsWithRelationships;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->FindComponentByClass<UNPCRelationshipComponent>())
		{
			AllActorsWithRelationships.Add(Actor);
		}
	}

	// Serialize each relationship
	for (AActor* Subject : AllActorsWithRelationships)
	{
		TArray<FRelationshipData> SubjectRelationships = Subsystem->GetRelationshipsAsSubject(Subject);
		
		for (const FRelationshipData& RelData : SubjectRelationships)
		{
			if (!RelData.IsValid())
			{
				continue;
			}

			FSerializedRelationship SerRel;
			SerRel.SubjectActorName = Subject->GetName();
			SerRel.TargetActorName = RelData.Target.Get()->GetName();

			// Serialize dimensions
			for (const auto& DimPair : RelData.Dimensions)
			{
				SerRel.DimensionValues.Add(DimPair.Key, DimPair.Value.Value);
			}

			// Serialize state
			SerRel.CurrentState = RelData.State.CurrentState;
			SerRel.TimeInState = RelData.State.GetTimeInState(Subsystem->GetWorld()->GetTimeSeconds());

			// Serialize traits
			for (const FRelationshipTraitInstance& TraitInst : RelData.Traits)
			{
				SerRel.ActiveTraits.Add(TraitInst.TraitTag);
				SerRel.TraitStackCounts.Add(TraitInst.TraitTag, TraitInst.StackCount);
			}

			// Serialize actions
			for (const auto& ActionPair : RelData.Actions)
			{
				SerRel.ActionUsageCounts.Add(ActionPair.Key, ActionPair.Value.UsageCount);
				SerRel.ActionLastExecutionTimes.Add(ActionPair.Key, ActionPair.Value.LastExecutionTime);
			}

			SerRel.LastUpdateTime = RelData.LastUpdateTime;

			SerializedRelationships.Add(SerRel);
		}
	}
	
	// Update metadata
	SaveVersion = CURRENT_VERSION;
	SaveTimestamp = FDateTime::Now();
	
	if (World)
	{
		SavedGameTime = World->GetTimeSeconds();
	}

	UE_LOG(LogRelationshipSystem, Log, TEXT("Serialized %d relationships"), SerializedRelationships.Num());
}

void URelationshipSaveGame::DeserializeToSubsystem(URelationshipSubsystem* Subsystem, UWorld* World)
{
	if (!Subsystem || !World)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Cannot deserialize: Subsystem or World is null"));
		return;
	}

	// Migrate if needed
	if (SaveVersion != CURRENT_VERSION)
	{
		if (!Migrate())
		{
			UE_LOG(LogRelationshipSystem, Error, TEXT("Failed to migrate save data from version %d to %d"), SaveVersion, CURRENT_VERSION);
			return;
		}
	}

	int32 LoadedCount = 0;
	int32 FailedCount = 0;

	for (const FSerializedRelationship& SerRel : SerializedRelationships)
	{
		// Find actors
		AActor* Subject = URelationshipSaveGameHelper::FindActorByName(World, SerRel.SubjectActorName);
		AActor* Target = URelationshipSaveGameHelper::FindActorByName(World, SerRel.TargetActorName);

		if (!Subject || !Target)
		{
			UE_LOG(LogRelationshipSystem, Warning, TEXT("Cannot restore relationship: Actor not found (%s -> %s)"),
				*SerRel.SubjectActorName, *SerRel.TargetActorName);
			FailedCount++;
			continue;
		}

		// Create relationship if doesn't exist
		if (!Subsystem->HasRelationship(Subject, Target))
		{
			Subsystem->CreateRelationship(Subject, Target, nullptr, nullptr);
		}

		// Restore dimension values
		for (const auto& DimPair : SerRel.DimensionValues)
		{
			Subsystem->SetDimensionValue(Subject, Target, DimPair.Key, DimPair.Value);
		}

		// Restore state
		if (SerRel.CurrentState.IsValid())
		{
			Subsystem->ForceStateChange(Subject, Target, SerRel.CurrentState);
		}

		// Restore traits
		for (const FGameplayTag& TraitTag : SerRel.ActiveTraits)
		{
			Subsystem->AddTrait(Subject, Target, TraitTag);
		}

		LoadedCount++;
	}

	UE_LOG(LogRelationshipSystem, Log, TEXT("Loaded %d relationships (%d failed)"), LoadedCount, FailedCount);
}

bool URelationshipSaveGame::Migrate()
{
	UE_LOG(LogRelationshipSystem, Log, TEXT("Migrating save from version %d to %d"), SaveVersion, CURRENT_VERSION);

	// Add migration logic here when save format changes
	// For now, version 1 is current, no migration needed

	SaveVersion = CURRENT_VERSION;
	return true;
}

bool URelationshipSaveGame::Validate(TArray<FText>& OutErrors) const
{
	bool bIsValid = true;

	if (SaveVersion > CURRENT_VERSION)
	{
		OutErrors.Add(FText::FromString(FString::Printf(
			TEXT("Save version %d is newer than current version %d"),
			SaveVersion, CURRENT_VERSION)));
		bIsValid = false;
	}

	if (SerializedRelationships.Num() == 0)
	{
		OutErrors.Add(FText::FromString(TEXT("No relationships in save file")));
		// Not critical, might be intentional
	}

	for (const FSerializedRelationship& SerRel : SerializedRelationships)
	{
		if (SerRel.SubjectActorName.IsEmpty() || SerRel.TargetActorName.IsEmpty())
		{
			OutErrors.Add(FText::FromString(TEXT("Relationship has empty actor name")));
			bIsValid = false;
		}
	}

	return bIsValid;
}

// URelationshipSaveGameHelper implementation

FString URelationshipSaveGameHelper::GetSlotName(const FString& SlotName)
{
	return SlotName.IsEmpty() ? DefaultSlotName : SlotName;
}

bool URelationshipSaveGameHelper::SaveRelationships(UObject* WorldContextObject, const FString& SlotName)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("RelationshipSubsystem not found"));
		return false;
	}

	// Create save game object
	URelationshipSaveGame* SaveGameInstance = Cast<URelationshipSaveGame>(
		UGameplayStatics::CreateSaveGameObject(URelationshipSaveGame::StaticClass()));

	if (!SaveGameInstance)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Failed to create save game object"));
		return false;
	}

	// Serialize relationships
	SaveGameInstance->SerializeFromSubsystem(Subsystem);

	// Save to disk
	FString ActualSlotName = GetSlotName(SlotName);
	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, ActualSlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogRelationshipSystem, Log, TEXT("Saved relationships to slot: %s"), *ActualSlotName);
	}
	else
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Failed to save relationships to slot: %s"), *ActualSlotName);
	}

	return bSuccess;
}

bool URelationshipSaveGameHelper::LoadRelationships(UObject* WorldContextObject, const FString& SlotName)
{
	if (!WorldContextObject)
	{
		return false;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return false;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return false;
	}

	URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("RelationshipSubsystem not found"));
		return false;
	}

	// Load from disk
	FString ActualSlotName = GetSlotName(SlotName);
	
	if (!UGameplayStatics::DoesSaveGameExist(ActualSlotName, 0))
	{
		UE_LOG(LogRelationshipSystem, Warning, TEXT("Save file does not exist: %s"), *ActualSlotName);
		return false;
	}

	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(ActualSlotName, 0);
	URelationshipSaveGame* SaveGameInstance = Cast<URelationshipSaveGame>(LoadedGame);

	if (!SaveGameInstance)
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Failed to load save game or invalid format"));
		return false;
	}

	// Validate
	TArray<FText> ValidationErrors;
	if (!SaveGameInstance->Validate(ValidationErrors))
	{
		UE_LOG(LogRelationshipSystem, Error, TEXT("Save validation failed"));
		for (const FText& Error : ValidationErrors)
		{
			UE_LOG(LogRelationshipSystem, Error, TEXT("  - %s"), *Error.ToString());
		}
		return false;
	}

	// Deserialize relationships
	SaveGameInstance->DeserializeToSubsystem(Subsystem, World);

	UE_LOG(LogRelationshipSystem, Log, TEXT("Loaded relationships from slot: %s"), *ActualSlotName);
	return true;
}

bool URelationshipSaveGameHelper::DoesSaveExist(const FString& SlotName)
{
	FString ActualSlotName = GetSlotName(SlotName);
	return UGameplayStatics::DoesSaveGameExist(ActualSlotName, 0);
}

bool URelationshipSaveGameHelper::DeleteSave(const FString& SlotName)
{
	FString ActualSlotName = GetSlotName(SlotName);
	bool bSuccess = UGameplayStatics::DeleteGameInSlot(ActualSlotName, 0);

	if (bSuccess)
	{
		UE_LOG(LogRelationshipSystem, Log, TEXT("Deleted save slot: %s"), *ActualSlotName);
	}

	return bSuccess;
}

TArray<FString> URelationshipSaveGameHelper::GetAllSaveSlots()
{
	TArray<FString> SaveSlots;
	
	// This is a simplified implementation
	// In production, you'd scan the save directory for all relationship save files
	
	if (DoesSaveExist(DefaultSlotName))
	{
		SaveSlots.Add(DefaultSlotName);
	}

	return SaveSlots;
}

AActor* URelationshipSaveGameHelper::FindActorByName(UWorld* World, const FString& ActorName)
{
	if (!World || ActorName.IsEmpty())
	{
		return nullptr;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->GetName() == ActorName)
		{
			return Actor;
		}
	}

	return nullptr;
}
