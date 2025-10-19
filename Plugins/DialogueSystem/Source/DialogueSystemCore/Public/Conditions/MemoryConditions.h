// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "MemoryConditions.generated.h"

// Forward declarations
class UDialogueSessionContext;
class UNPCMemoryComponent;
class AActor;
enum class EMemoryType : uint8;
enum class EMemoryEmotion : uint8;
struct FNPCMemoryEntry;
struct FConversationTopic;

/**
 * Базовый класс для условия диалога, работающего с памятью NPC
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UMemoryConditionBase : public UObject
{
    GENERATED_BODY()

public:
    /** Вычислить условие для NPC */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Condition")
    bool EvaluateCondition(AActor* NPCActor) const;
    virtual bool EvaluateCondition_Implementation(AActor* NPCActor) const { return true; }
};

/**
 * Проверяет наличие памяти с определенным тегом
 */
UCLASS(DisplayName = "Has Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UHasMemoryCondition : public UMemoryConditionBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для проверки */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTag MemoryTag;

	virtual bool EvaluateCondition_Implementation(AActor* NPCActor) const override;
};

/**
 * Проверяет силу памяти
 */
UENUM(BlueprintType)
enum class EMemoryComparisonType : uint8
{
	GreaterThan,
	GreaterOrEqual,
	Equal,
	LessOrEqual,
	LessThan
};

UCLASS(DisplayName = "Memory Strength", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UMemoryStrengthCondition : public UMemoryConditionBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для проверки */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTag MemoryTag;

	/** Тип сравнения */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EMemoryComparisonType ComparisonType = EMemoryComparisonType::GreaterOrEqual;

	/** Целевая сила памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TargetStrength = 0.5f;

	virtual bool EvaluateCondition_Implementation(AActor* NPCActor) const override;
};

/**
 * Проверяет деградацию памяти
 */
UCLASS(DisplayName = "Memory Decay", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UMemoryDecayCondition : public UMemoryConditionBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для проверки */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTag MemoryTag;

	/** Порог деградации */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DecayThreshold = 0.3f;

	/** Проверять что память деградировала (true) или еще не деградировала (false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	bool bCheckForDecayed = true;

	virtual bool EvaluateCondition_Implementation(AActor* NPCActor) const override;
};

/**
 * Проверяет недавность памяти
 */
UCLASS(DisplayName = "Recent Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API URecentMemoryCondition : public UMemoryConditionBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для проверки */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FGameplayTag MemoryTag;

	/** Максимальное время с последнего обновления (в секундах) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	float MaxTimeSinceUpdate = 300.0f; // 5 минут по умолчанию

	virtual bool EvaluateCondition_Implementation(AActor* NPCActor) const override;
};
