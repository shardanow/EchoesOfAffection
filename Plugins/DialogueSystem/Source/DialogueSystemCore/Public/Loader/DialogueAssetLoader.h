// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "DialogueAssetLoader.generated.h"

// Forward declarations
class UDialogueDataAsset;

/**
 * Async callback for dialogue asset loading
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDialogueAssetLoaded, UDialogueDataAsset*, LoadedAsset, bool, bSuccess);

/**
 * Async loader for DialogueDataAssets
 * Provides non-blocking asset loading with callbacks
 * 
 * Performance benefits:
 * - No frame hitches during loading
 * - Better streaming experience
 * - Can preload during level streaming
 * - Supports priority-based loading
 */
UCLASS()
class DIALOGUESYSTEMCORE_API UDialogueAssetLoader : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Load dialogue asset asynchronously
     * @param AssetPath Soft object path to the asset
     * @param OnLoaded Callback when loading completes
     * @param Priority Loading priority (higher = more important)
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loader")
    void LoadDialogueAsync(
        const TSoftObjectPtr<UDialogueDataAsset>& AssetPath,
        FOnDialogueAssetLoaded OnLoaded,
        int32 Priority = 0
    );

    /**
     * Load multiple dialogue assets asynchronously
     * @param AssetPaths Array of soft object paths
     * @param OnAllLoaded Callback when all assets are loaded
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loader")
    void LoadDialoguesAsync(
        const TArray<TSoftObjectPtr<UDialogueDataAsset>>& AssetPaths,
        FOnDialogueAssetLoaded OnAllLoaded
    );

    /**
     * Cancel pending async load
     * @param AssetPath The asset path to cancel
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue|Loader")
    void CancelLoad(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath);

    /**
     * Check if asset is currently loading
     * @param AssetPath The asset path to check
     * @return true if loading is in progress
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Loader")
    bool IsLoading(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath) const;

    /**
     * Get loading progress (0.0 to 1.0)
     * @param AssetPath The asset path to check
     * @return Progress value or -1.0 if not loading
     */
    UFUNCTION(BlueprintPure, Category = "Dialogue|Loader")
    float GetLoadingProgress(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath) const;

protected:
    /** Streamable manager for async loading */
    FStreamableManager StreamableManager;

    /** Active loading handles */
    TMap<FSoftObjectPath, TSharedPtr<FStreamableHandle>> ActiveHandles;

    /**
     * Internal callback when asset loads
     */
    void OnAssetLoadedInternal(
        FSoftObjectPath AssetPath,
        FOnDialogueAssetLoaded Callback
    );
};
