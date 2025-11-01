// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestSaveGame.h"
#include "QuestSaveManager.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSaveCompleteCallback, bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnLoadCompleteCallback, bool, bSuccess, UQuestSaveGame*, SaveGame);

/**
 * Quest Save Manager - Handles save/load operations
 * Single Responsibility: Persistent storage management
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestSaveManager : public UObject
{
	GENERATED_BODY()

public:
	UQuestSaveManager();

	void Initialize();
	void Shutdown();

	// === Save/Load ===

	/**
	 * Save quest progress asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void SaveQuestProgressAsync(UQuestSaveGame* SaveGame, FOnSaveCompleteCallback OnComplete);

	/**
	 * Load quest progress asynchronously
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void LoadQuestProgressAsync(const FString& SlotName, int32 UserIndex, FOnLoadCompleteCallback OnComplete);

	/**
	 * Save synchronously (blocking)
	 * @deprecated Use SaveQuestProgressAsync instead
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save", meta = (DeprecatedFunction, DeprecationMessage = "Use SaveQuestProgressAsync instead"))
	bool SaveQuestProgress(UQuestSaveGame* SaveGame);

	/**
	 * Load synchronously (blocking)
	 * @deprecated Use LoadQuestProgressAsync instead
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save", meta = (DeprecatedFunction, DeprecationMessage = "Use LoadQuestProgressAsync instead"))
	UQuestSaveGame* LoadQuestProgress(const FString& SlotName, int32 UserIndex);

	/**
	 * Delete save slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	bool DeleteSaveSlot(const FString& SlotName, int32 UserIndex);

	/**
	 * Check if save slot exists
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Save")
	bool DoesSaveSlotExist(const FString& SlotName, int32 UserIndex) const;

	// === Auto-Save ===

	/**
	 * Enable auto-save with interval
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void EnableAutoSave(float IntervalSeconds);

	/**
	 * Disable auto-save
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Save")
	void DisableAutoSave();

	/**
	 * Check if auto-save is enabled
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Save")
	bool IsAutoSaveEnabled() const { return bAutoSaveEnabled; }

	// === Configuration ===

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Save")
	FString DefaultSlotName = TEXT("QuestSaveSlot");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Save")
	int32 DefaultUserIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Save")
	float AutoSaveInterval = 60.0f;

protected:
	UPROPERTY()
	TObjectPtr<UQuestSaveGame> CurrentSaveGame;

	bool bAutoSaveEnabled = false;
	FTimerHandle AutoSaveTimerHandle;

	void OnAutoSave();

	/** Helper function to get full save file path */
	FString GetSaveFilePath(const FString& SlotName) const;
};
