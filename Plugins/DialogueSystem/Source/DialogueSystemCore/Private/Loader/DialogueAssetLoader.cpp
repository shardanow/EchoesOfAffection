// Copyright Epic Games, Inc. All Rights Reserved.

#include "Loader/DialogueAssetLoader.h"
#include "Data/DialogueDataAsset.h"
#include "Engine/StreamableManager.h"

void UDialogueAssetLoader::LoadDialogueAsync(
    const TSoftObjectPtr<UDialogueDataAsset>& AssetPath,
    FOnDialogueAssetLoaded OnLoaded,
    int32 Priority)
{
    if (AssetPath.IsNull())
    {
        // Invalid path - callback with failure
        if (OnLoaded.IsBound())
        {
            OnLoaded.Execute(nullptr, false);
        }
        return;
    }

    // Check if already loaded
    if (AssetPath.IsValid())
    {
        // Already loaded - callback immediately
        if (OnLoaded.IsBound())
        {
            OnLoaded.Execute(AssetPath.Get(), true);
        }
        return;
    }

    FSoftObjectPath SoftPath = AssetPath.ToSoftObjectPath();

    // Check if already loading
    if (ActiveHandles.Contains(SoftPath))
    {
        // Already loading - wait for existing handle
        // You could queue multiple callbacks here if needed
        return;
    }

    // Start async load
    FStreamableDelegate LoadDelegate;
    LoadDelegate.BindUObject(this, &UDialogueAssetLoader::OnAssetLoadedInternal, SoftPath, OnLoaded);

    TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
        SoftPath,
        LoadDelegate,
        Priority
    );

    if (Handle.IsValid())
    {
        ActiveHandles.Add(SoftPath, Handle);
    }
}

void UDialogueAssetLoader::LoadDialoguesAsync(
    const TArray<TSoftObjectPtr<UDialogueDataAsset>>& AssetPaths,
    FOnDialogueAssetLoaded OnAllLoaded)
{
    if (AssetPaths.Num() == 0)
    {
        if (OnAllLoaded.IsBound())
        {
            OnAllLoaded.Execute(nullptr, false);
        }
        return;
    }

    // Convert to soft object paths
    TArray<FSoftObjectPath> SoftPaths;
    for (const TSoftObjectPtr<UDialogueDataAsset>& AssetPath : AssetPaths)
    {
        if (!AssetPath.IsNull())
        {
            SoftPaths.Add(AssetPath.ToSoftObjectPath());
        }
    }

    if (SoftPaths.Num() == 0)
    {
        if (OnAllLoaded.IsBound())
        {
            OnAllLoaded.Execute(nullptr, false);
        }
        return;
    }

    // Load all assets
    FStreamableDelegate LoadDelegate;
    LoadDelegate.BindLambda([OnAllLoaded, SoftPaths]()
    {
        // All assets loaded
        if (OnAllLoaded.IsBound())
        {
            // You could return array of loaded assets here
            OnAllLoaded.Execute(nullptr, true);
        }
    });

    TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
        SoftPaths,
        LoadDelegate
    );

    // Store handle for each path
    for (const FSoftObjectPath& Path : SoftPaths)
    {
        ActiveHandles.Add(Path, Handle);
    }
}

void UDialogueAssetLoader::CancelLoad(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath)
{
    if (AssetPath.IsNull())
    {
        return;
    }

    FSoftObjectPath SoftPath = AssetPath.ToSoftObjectPath();
    
    if (TSharedPtr<FStreamableHandle>* HandlePtr = ActiveHandles.Find(SoftPath))
    {
        if (HandlePtr->IsValid())
        {
            (*HandlePtr)->CancelHandle();
        }
        ActiveHandles.Remove(SoftPath);
    }
}

bool UDialogueAssetLoader::IsLoading(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath) const
{
    if (AssetPath.IsNull())
    {
        return false;
    }

    FSoftObjectPath SoftPath = AssetPath.ToSoftObjectPath();
    
    if (const TSharedPtr<FStreamableHandle>* HandlePtr = ActiveHandles.Find(SoftPath))
    {
        return HandlePtr->IsValid() && (*HandlePtr)->IsLoadingInProgress();
    }

    return false;
}

float UDialogueAssetLoader::GetLoadingProgress(const TSoftObjectPtr<UDialogueDataAsset>& AssetPath) const
{
    if (AssetPath.IsNull())
    {
        return -1.0f;
    }

    FSoftObjectPath SoftPath = AssetPath.ToSoftObjectPath();
    
    if (const TSharedPtr<FStreamableHandle>* HandlePtr = ActiveHandles.Find(SoftPath))
    {
        if (HandlePtr->IsValid())
        {
            return (*HandlePtr)->GetProgress();
        }
    }

    return -1.0f;
}

void UDialogueAssetLoader::OnAssetLoadedInternal(
    FSoftObjectPath AssetPath,
    FOnDialogueAssetLoaded Callback)
{
    // Remove from active handles
    TSharedPtr<FStreamableHandle> Handle;
    if (ActiveHandles.RemoveAndCopyValue(AssetPath, Handle))
    {
        // Get loaded asset
        UObject* LoadedObject = Handle->GetLoadedAsset();
        UDialogueDataAsset* LoadedDialogue = Cast<UDialogueDataAsset>(LoadedObject);

        // Execute callback
        if (Callback.IsBound())
        {
            Callback.Execute(LoadedDialogue, LoadedDialogue != nullptr);
        }
    }
    else
    {
        // Handle not found - callback with failure
        if (Callback.IsBound())
        {
            Callback.Execute(nullptr, false);
        }
    }
}
