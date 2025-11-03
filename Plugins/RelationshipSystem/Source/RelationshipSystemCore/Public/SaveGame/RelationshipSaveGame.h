// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "RelationshipSaveGame.generated.h"

/**
 * Serialized relationship data for save/load
 */
USTRUCT()
struct FSerializedRelationship
{
	GENERATED_BODY()

	/** Subject actor name (for lookup) */
	UPROPERTY()
	FString SubjectActorName;

	/** Target actor name (for lookup) */
	UPROPERTY()
	FString TargetActorName;

	/** Dimension values */
	UPROPERTY()
	TMap<FGameplayTag, float> DimensionValues;

	/** Current state */
	UPROPERTY()
	FGameplayTag CurrentState;

	/** Time in state */
	UPROPERTY()
	float TimeInState;

	/** Active traits */
	UPROPERTY()
	TArray<FGameplayTag> ActiveTraits;

	/** Trait stack counts */
	UPROPERTY()
	TMap<FGameplayTag, int32> TraitStackCounts;

	/** Action usage counts */
	UPROPERTY()
	TMap<FGameplayTag, int32> ActionUsageCounts;

	/** Action last execution times */
	UPROPERTY()
	TMap<FGameplayTag, float> ActionLastExecutionTimes;

	/** Last update time */
	UPROPERTY()
	float LastUpdateTime;

	FSerializedRelationship()
		: TimeInState(0.0f)
		, LastUpdateTime(0.0f)
	{
	}
};

/**
 * Save game for relationship system
 * Stores all active relationships with versioning support
 */
UCLASS()
class RELATIONSHIPSYSTEMCORE_API URelationshipSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	URelationshipSaveGame();

	/** Save format version (for migration) */
	UPROPERTY()
	int32 SaveVersion;

	/** All serialized relationships */
	UPROPERTY()
	TArray<FSerializedRelationship> SerializedRelationships;

	/** Save timestamp */
	UPROPERTY()
	FDateTime SaveTimestamp;

	/** Game time when saved */
	UPROPERTY()
	float SavedGameTime;

	/** Current version number */
	static constexpr int32 CURRENT_VERSION = 1;

	/** Serialize relationships from subsystem */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void SerializeFromSubsystem(class URelationshipSubsystem* Subsystem);

	/** Deserialize relationships to subsystem */
	UFUNCTION(BlueprintCallable, Category = "Save")
	void DeserializeToSubsystem(class URelationshipSubsystem* Subsystem, UWorld* World);

	/** Migrate old save format to current (if needed) */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool Migrate();

	/** Validate save data */
	UFUNCTION(BlueprintCallable, Category = "Save")
	bool Validate(TArray<FText>& OutErrors) const;

	/** Get total number of relationships */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Save")
	int32 GetRelationshipCount() const { return SerializedRelationships.Num(); }
};

/**
 * Helper class for save/load operations
 */
UCLASS()
class RELATIONSHIPSYSTEMCORE_API URelationshipSaveGameHelper : public UObject
{
	GENERATED_BODY()

public:
	/** Default slot name */
	static const FString DefaultSlotName;

	/** Save relationships to slot */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Save", meta = (WorldContext = "WorldContextObject"))
	static bool SaveRelationships(UObject* WorldContextObject, const FString& SlotName = "");

	/** Load relationships from slot */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Save", meta = (WorldContext = "WorldContextObject"))
	static bool LoadRelationships(UObject* WorldContextObject, const FString& SlotName = "");

	/** Check if save exists */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Relationships|Save")
	static bool DoesSaveExist(const FString& SlotName = "");

	/** Delete save */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Save")
	static bool DeleteSave(const FString& SlotName = "");

	/** Get all save slots */
	UFUNCTION(BlueprintCallable, Category = "Relationships|Save")
	static TArray<FString> GetAllSaveSlots();

public: /** Get actual slot name */
	static FString GetSlotName(const FString& SlotName);

	/** Find actor by name in world */
	static AActor* FindActorByName(UWorld* World, const FString& ActorName);
};
