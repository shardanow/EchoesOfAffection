// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "QuestEventBridge.generated.h"

// Forward declarations
class UGameEventBusSubsystem;
struct FGameEventPayload;
struct FQuestEventPayload;

/**
 * Quest Event Bridge
 * 
 * Связывает GameEventBus с QuestSystem без жёстких зависимостей
 * 
 * Принцип работы:
 * 1) Инициализируется автоматически при старте Quest System
 * 2) Подписывается на нужные события из GameEventBus
 * 3) Транслирует их в QuestEventBus
 * 
 * Пример:
 * - GameEventBus эмитирует "Inventory.Event.ItemAcquired"
 * - Bridge ловит это событие
 * - Транслирует в QuestEventBus как OnItemAcquired
 * - Quest objectives получают обновление
 * 
 * Dependency Direction:
 * QuestSystem -> GameEventBus (one-way, through this bridge)
 * DialogueSystem -> GameEventBus (independent)
 * InventorySystem -> GameEventBus (independent)
 */
UCLASS()
class QUESTSYSTEMRUNTIME_API UQuestEventBridge : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Initialize bridge
	 * @param QuestSubsystem The quest subsystem to forward events to
	 */
	UFUNCTION()
	void Initialize(UObject* QuestSubsystem);

	/**
	 * Shutdown bridge
	 */
	UFUNCTION()
	void Shutdown();

	/**
	 * Check if bridge is active
	 */
	bool IsActive() const { return bIsActive; }

protected:
	/**
	 * Handle generic game event from EventBus
	 * Routes to appropriate Quest event handler
	 */
	UFUNCTION()
	void OnGameEvent(const FGameEventPayload& Payload);

private:
	/** Is bridge initialized and active? */
	bool bIsActive = false;

	/** Reference to Quest Subsystem (weak to avoid circular refs) */
	TWeakObjectPtr<UObject> QuestSubsystemRef;

	/** Reference to GameEventBus */
	TWeakObjectPtr<UGameEventBusSubsystem> EventBusRef;

	/** Registered delegate handles (for cleanup) */
	TArray<FDelegateHandle> DelegateHandles;

	/** Helper: Convert GameEventPayload to QuestEventPayload */
	void ForwardToQuestSystem(const FGameEventPayload& GamePayload);

	/** Helper: Subscribe to event tags */
	void SubscribeToEventTags();
};
