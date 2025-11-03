// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

// TEMPORARY: Bridge disabled - no generated.h needed
// #include "RelationshipSystemBridge.generated.h"

// Forward declarations
class URelationshipSubsystem;
class UMemoryToRelationshipMapping;

// TEMPORARY: Bridge commented out until GameEventBus integration is complete
// TODO: Uncomment when FGameEvent struct is available from GameEventBus plugin

#if 0 // Disabled for now

/**
 * Bridge для интеграции между системами через GameEventBus
 * 
 * Функции:
 * 1. NPCMemory ? RelationshipSystem (память влияет на отношения)
 * 2. RelationshipSystem ? NPCMemory (изменения создают воспоминания)
 * 3. Полностью data-driven через UMemoryToRelationshipMapping
 * 
 * Soft dependency:
 * - Проверяет наличие GameEventBus runtime
 * - Может работать без NPCMemoryComponent
 * - Конфигурируется через Data Assets
 */
UCLASS()
class RELATIONSHIPSYSTEMCORE_API URelationshipSystemBridge : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	//~ Begin USubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End USubsystem Interface

protected:
	/** Проверяем доступность GameEventBus */
	bool bGameEventBusAvailable = false;

	/** Кешированный RelationshipSubsystem */
	UPROPERTY(Transient)
	TObjectPtr<URelationshipSubsystem> RelationshipSubsystem;

	/** Data Asset с маппингом Memory ? Relationship */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Integration", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMemoryToRelationshipMapping> MappingConfig;

	/** Таймер для периодической синхронизации */
	float TimeSinceLastSync = 0.0f;

protected:
	/** Настройка слушателей событий */
	void SetupEventListeners();

	/** Очистка слушателей */
	void CleanupEventListeners();

	//~ Begin GameEventBus Listeners

	/** Callback: событие создания памяти */
	UFUNCTION()
	void OnMemoryEventReceived(const FGameEvent& Event);

	//~ End GameEventBus Listeners

	//~ Begin RelationshipSubsystem Listeners

	/** Callback: изменение dimension */
	UFUNCTION()
	void OnDimensionChanged(AActor* Subject, AActor* Target, FGameplayTag DimensionTag, float NewValue);

	/** Callback: переход состояния */
	UFUNCTION()
	void OnStateTransitioned(AActor* Subject, AActor* Target, FGameplayTag OldState, FGameplayTag NewState);

	/** Callback: добавление trait */
	UFUNCTION()
	void OnTraitAdded(AActor* Subject, AActor* Target, FGameplayTag TraitTag);

	/** Callback: выполнение action */
	UFUNCTION()
	void OnActionExecuted(AActor* Subject, AActor* Target, FGameplayTag ActionTag);

	//~ End RelationshipSubsystem Listeners

protected:
	//~ Begin Memory ? Relationship Logic

	/**
	 * Конвертировать память в изменение отношений
	 * Использует MappingConfig для data-driven логики
	 */
	void ConvertMemoryToRelationshipChange(
		AActor* NPC, 
		AActor* Target,
		const FString& MemoryType, 
		float Importance,
		float Freshness,
		const FString& Emotion);

	/**
	 * Применить эмоциональные модификаторы к dimensions
	 */
	void ApplyEmotionModifiers(
		AActor* NPC,
		AActor* Target, 
		const FString& Emotion,
		float Importance);

	//~ End Memory ? Relationship Logic

	//~ Begin Relationship ? Memory Logic

	/**
	 * Создать воспоминание об изменении отношений
	 */
	void CreateMemoryFromRelationshipChange(
		AActor* NPC,
		AActor* Target,
		FGameplayTag DimensionTag,
		float NewValue);

	/**
	 * Создать воспоминание о смене состояния
	 */
	void CreateMemoryFromStateTransition(
		AActor* NPC,
		AActor* Target,
		FGameplayTag OldState,
		FGameplayTag NewState);

	/**
	 * Рассчитать важность воспоминания на основе изменения
	 */
	float CalculateMemoryImportance(FGameplayTag DimensionTag, float Value) const;

	/**
	 * Определить эмоцию для воспоминания
	 */
	FString DetermineMemoryEmotion(FGameplayTag DimensionTag, float Value) const;

	/**
	 * Сгенерировать описание воспоминания
	 */
	FText GenerateMemoryDescription(
		FGameplayTag DimensionTag, 
		float Value,
		AActor* Target) const;

	//~ End Relationship ? Memory Logic

	//~ Begin GameEventBus Integration

	/**
	 * Broadcast события для других систем
	 */
	void BroadcastRelationshipEvent(
		FGameplayTag EventTag,
		AActor* Subject,
		AActor* Target,
		const TMap<FName, FString>& Payload);

	//~ End GameEventBus Integration

public:
	/** Проверить доступность GameEventBus */
	UFUNCTION(BlueprintPure, Category = "Relationship System|Integration")
	bool IsGameEventBusAvailable() const;

	/** Установить конфиг маппинга */
	UFUNCTION(BlueprintCallable, Category = "Relationship System|Integration")
	void SetMappingConfig(UMemoryToRelationshipMapping* NewConfig) { MappingConfig = NewConfig; }

	/** Получить конфиг маппинга */
	UFUNCTION(BlueprintPure, Category = "Relationship System|Integration")
	UMemoryToRelationshipMapping* GetMappingConfig() const { return MappingConfig; }
};

#endif // Disabled Bridge
