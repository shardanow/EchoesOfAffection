// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MemoryToRelationshipMapping.generated.h"

/**
 * Dimension modifier based on memory emotion
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FEmotionDimensionModifier
{
	GENERATED_BODY()

	/** Dimension tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	FGameplayTag DimensionTag;

	/** Importance multiplier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier", meta=(ClampMin="0.0", ClampMax="2.0"))
	float ImportanceMultiplier = 1.0f;

	/** Positive influence flag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	bool bPositiveInfluence = true;

	FEmotionDimensionModifier()
		: ImportanceMultiplier(1.0f)
		, bPositiveInfluence(true)
	{}
};

/**
 * Memory type to relationship action mapping
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FMemoryActionMapping
{
	GENERATED_BODY()

	/** Memory type name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	FString MemoryTypeName;

	/** Action tag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping", meta=(Categories="Relationship.Action"))
	FGameplayTag ActionTag;

	/** Minimum importance threshold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping", meta=(ClampMin="0.0", ClampMax="100.0"))
	float MinImportance = 70.0f;

	FMemoryActionMapping()
		: MinImportance(70.0f)
	{}
};

/**
 * Emotion to dimension mapping
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FEmotionMapping
{
	GENERATED_BODY()

	/** Emotion name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	FString EmotionName;

	/** Dimension modifiers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	TArray<FEmotionDimensionModifier> DimensionModifiers;
};

/**
 * Data Asset for NPCMemory to RelationshipSystem integration mapping
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API UMemoryToRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Memory type action mappings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Memory Mapping")
	TArray<FMemoryActionMapping> MemoryTypeActions;

	/** Emotion mappings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Emotion Mapping")
	TArray<FEmotionMapping> EmotionMappings;

	/** Global minimum importance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(ClampMin="0.0", ClampMax="100.0"))
	float GlobalMinImportance = 60.0f;

	/** Minimum freshness threshold */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
	float MinFreshness = 0.4f;

	/** Enable memory to relationship sync */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bEnableMemoryToRelationship = true;

	/** Enable relationship to memory sync */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bEnableRelationshipToMemory = true;

	/** Sync interval in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta=(ClampMin="30.0", ClampMax="300.0"))
	float SyncInterval = 60.0f;

public:
	/** Find action for memory type */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	bool FindActionForMemoryType(const FString& MemoryType, float Importance, FGameplayTag& OutActionTag) const;

	/** Get modifiers for emotion */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	TArray<FEmotionDimensionModifier> GetModifiersForEmotion(const FString& EmotionName) const;

	/** Check if memory should affect relationship */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	bool ShouldMemoryAffectRelationship(float Importance, float Freshness) const;
};
