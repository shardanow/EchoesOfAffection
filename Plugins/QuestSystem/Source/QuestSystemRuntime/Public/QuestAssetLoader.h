// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StreamableManager.h"
#include "QuestAsset.h"
#include "QuestAssetLoader.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQuestAssetLoaded, UQuestAsset*, LoadedAsset);
DECLARE_DELEGATE_OneParam(FOnQuestAssetLoadedNative, UQuestAsset*);

/**
 * Quest Asset Loader - Handles async loading of quest assets
 * Separates loading logic from subsystem for better architecture
 */
UCLASS()
class QUESTSYSTEMRUNTIME_API UQuestAssetLoader : public UObject
{
	GENERATED_BODY()

public:
	UQuestAssetLoader();

	/**
	 * Load quest asset asynchronously via Asset Manager
	 * @param QuestId - Quest ID to load
	 * @param OnLoaded - Callback when loaded
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Loading")
	void LoadQuestAssetAsync(FName QuestId, FOnQuestAssetLoaded OnLoaded);

	/**
	 * Load quest asset asynchronously (native)
	 */
	void LoadQuestAssetAsyncNative(FName QuestId, FOnQuestAssetLoadedNative OnLoaded);

	/**
	 * Preload multiple quest assets at once
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Loading")
	void PreloadQuestAssets(const TArray<FName>& QuestIds, FOnQuestAssetLoaded OnAllLoaded);

	/**
	 * Cancel all pending loads
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Loading")
	void CancelAllLoads();

	/**
	 * Get quest asset if already loaded (doesn't trigger load)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Loading")
	UQuestAsset* GetLoadedQuestAsset(FName QuestId) const;

	/**
	 * Check if quest is currently loading
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Loading")
	bool IsQuestLoading(FName QuestId) const;

protected:
	/** Cache of loaded quest assets */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UQuestAsset>> LoadedQuests;

	/** Active loading handles */
	TMap<FName, TSharedPtr<FStreamableHandle>> LoadingHandles;

	/** Streamable manager reference */
	FStreamableManager* StreamableManager;

	/** Get or create streamable manager */
	FStreamableManager& GetStreamableManager();

	/** Internal callback when asset loads */
	void OnQuestAssetLoadComplete(FName QuestId, FOnQuestAssetLoadedNative OnLoaded);
};
