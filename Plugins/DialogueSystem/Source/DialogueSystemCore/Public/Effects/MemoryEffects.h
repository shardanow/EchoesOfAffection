// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Effects/DialogueEffectExecutor.h"
#include "MemoryEffects.generated.h"

// Forward declarations
class UDialogueSessionContext;
class AActor;

/**
 * Базовый класс эффекта для работы с памятью NPC
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DIALOGUESYSTEMCORE_API UMemoryEffectBase : public UObject
{
    GENERATED_BODY()

public:
    /** Выполнить эффект */
    UFUNCTION(BlueprintNativeEvent, Category = "Dialogue|Effect")
    void Execute(AActor* NPCActor, AActor* PlayerActor) const;
    virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const {}
};

/**
 * Добавляет память с определенным тегом
 */
UCLASS(DisplayName = "Add Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UAddMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	/** Тег памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	/** Сила памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MemoryStrength = 1.0f;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 * Модифицирует силу существующей памяти
 */
UCLASS(DisplayName = "Modify Memory Strength", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API UModifyMemoryStrengthEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	/** Тег памяти */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	/** Модификатор силы */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float StrengthModifier = 0.1f;

	/** Добавить к текущей силе (true) или установить новую (false) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bAdditive = true;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 * Удаляет память
 */
UCLASS(DisplayName = "Remove Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API URemoveMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для удаления */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};

/**
 * Обновляет timestamp памяти (делает её "свежей")
 */
UCLASS(DisplayName = "Refresh Memory", meta = (Category = "Memory"))
class DIALOGUESYSTEMCORE_API URefreshMemoryEffect : public UMemoryEffectBase
{
	GENERATED_BODY()

public:
	/** Тег памяти для обновления */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FGameplayTag MemoryTag;

	virtual void Execute_Implementation(AActor* NPCActor, AActor* PlayerActor) const override;
};
