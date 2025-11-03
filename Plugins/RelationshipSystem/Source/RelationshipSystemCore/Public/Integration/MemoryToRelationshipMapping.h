// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MemoryToRelationshipMapping.generated.h"

/**
 * Модификаторы dimension на основе эмоции памяти
 * Конфигурируются в Data Asset, без хардкода
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FEmotionDimensionModifier
{
	GENERATED_BODY()

	/** Какую dimension затрагивает (Trust, Affinity, Romance, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	FGameplayTag DimensionTag;

	/** Множитель влияния важности памяти (0.0 - 2.0) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier", meta=(ClampMin="0.0", ClampMax="2.0"))
	float ImportanceMultiplier = 1.0f;

	/** Направление влияния */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Modifier")
	bool bPositiveInfluence = true;

	FEmotionDimensionModifier()
		: ImportanceMultiplier(1.0f)
		, bPositiveInfluence(true)
	{}
};

/**
 * Маппинг типа памяти на действие в отношениях
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FMemoryActionMapping
{
	GENERATED_BODY()

	/** Тип памяти (из NPCMemoryComponent enum, передаётся как string) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	FString MemoryTypeName;

	/** Действие в RelationshipSystem */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping", meta=(Categories="Relationship.Action"))
	FGameplayTag ActionTag;

	/** Минимальная важность для срабатывания */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping", meta=(ClampMin="0.0", ClampMax="100.0"))
	float MinImportance = 70.0f;

	FMemoryActionMapping()
		: MinImportance(70.0f)
	{}
};

/**
 * Маппинг эмоции на модификаторы dimensions
 */
USTRUCT(BlueprintType)
struct RELATIONSHIPSYSTEMCORE_API FEmotionMapping
{
	GENERATED_BODY()

	/** Эмоция памяти (из NPCMemoryComponent enum) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	FString EmotionName;

	/** Список модификаторов для разных dimensions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mapping")
	TArray<FEmotionDimensionModifier> DimensionModifiers;
};

/**
 * Data Asset для конфигурации интеграции NPCMemory ? RelationshipSystem
 * 
 * Полностью data-driven, без хардкода в C++
 * Дизайнеры могут настраивать через Editor
 */
UCLASS(BlueprintType)
class RELATIONSHIPSYSTEMCORE_API UMemoryToRelationshipMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Маппинг типов памяти на действия */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Memory Mapping")
	TArray<FMemoryActionMapping> MemoryTypeActions;

	/** Маппинг эмоций на модификаторы dimensions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Emotion Mapping")
	TArray<FEmotionMapping> EmotionMappings;

	/** Минимальная важность памяти для влияния на отношения */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(ClampMin="0.0", ClampMax="100.0"))
	float GlobalMinImportance = 60.0f;

	/** Минимальная свежесть памяти (0.0 - 1.0) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(ClampMin="0.0", ClampMax="1.0"))
	float MinFreshness = 0.4f;

	/** Включить автоматическую синхронизацию Memory ? Relationship */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bEnableMemoryToRelationship = true;

	/** Включить автоматическое создание памяти при изменении отношений */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bEnableRelationshipToMemory = true;

	/** Интервал синхронизации (секунды) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Performance", meta=(ClampMin="30.0", ClampMax="300.0"))
	float SyncInterval = 60.0f;

public:
	/** Найти действие для типа памяти */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	bool FindActionForMemoryType(const FString& MemoryType, float Importance, FGameplayTag& OutActionTag) const;

	/** Получить модификаторы для эмоции */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	TArray<FEmotionDimensionModifier> GetModifiersForEmotion(const FString& EmotionName) const;

	/** Проверить, должна ли память влиять на отношения */
	UFUNCTION(BlueprintPure, Category = "Memory Mapping")
	bool ShouldMemoryAffectRelationship(float Importance, float Freshness) const;
};
