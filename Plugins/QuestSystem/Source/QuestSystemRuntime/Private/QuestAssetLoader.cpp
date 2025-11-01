// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestAssetLoader.h"
#include "Engine/AssetManager.h"

UQuestAssetLoader::UQuestAssetLoader()
{
	StreamableManager = nullptr;
}

FStreamableManager& UQuestAssetLoader::GetStreamableManager()
{
	if (!StreamableManager)
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		StreamableManager = &AssetManager.GetStreamableManager();
	}
	return *StreamableManager;
}

void UQuestAssetLoader::LoadQuestAssetAsync(FName QuestId, FOnQuestAssetLoaded OnLoaded)
{
	LoadQuestAssetAsyncNative(QuestId, FOnQuestAssetLoadedNative::CreateLambda([OnLoaded](UQuestAsset* Asset)
	{
		OnLoaded.ExecuteIfBound(Asset);
	}));
}

void UQuestAssetLoader::LoadQuestAssetAsyncNative(FName QuestId, FOnQuestAssetLoadedNative OnLoaded)
{
	// Check cache first
	if (const TObjectPtr<UQuestAsset>* CachedAsset = LoadedQuests.Find(QuestId))
	{
		UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Quest '%s' already loaded from cache"), *QuestId.ToString());
		OnLoaded.ExecuteIfBound(CachedAsset->Get());
		return;
	}

	// Check if already loading
	if (LoadingHandles.Contains(QuestId))
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestAssetLoader: Quest '%s' is already loading"), *QuestId.ToString());
		return;
	}

	// Get asset data from Asset Manager
	FPrimaryAssetId AssetId(FPrimaryAssetType("Quest"), QuestId);
	UAssetManager& AssetManager = UAssetManager::Get();

	FAssetData AssetData;
	if (!AssetManager.GetPrimaryAssetData(AssetId, AssetData))
	{
		UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: Quest '%s' not registered in Asset Manager! Make sure the quest asset is set up with PrimaryAssetType 'Quest'"), *QuestId.ToString());
		OnLoaded.ExecuteIfBound(nullptr);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Starting async load for quest '%s' from path '%s'"), *QuestId.ToString(), *AssetData.GetSoftObjectPath().ToString());

	// Start async load
	FSoftObjectPath AssetPath = AssetData.GetSoftObjectPath();
	TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(
		AssetPath,
		[this, QuestId, OnLoaded, AssetPath]()
		{
			UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Async load callback triggered for quest '%s'"), *QuestId.ToString());
			OnQuestAssetLoadComplete(QuestId, OnLoaded);
		},
		FStreamableManager::AsyncLoadHighPriority
	);

	if (Handle.IsValid())
	{
		LoadingHandles.Add(QuestId, Handle);
		UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Streamable handle created for quest '%s'"), *QuestId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: Failed to create streamable handle for quest '%s'"), *QuestId.ToString());
		OnLoaded.ExecuteIfBound(nullptr);
	}
}

void UQuestAssetLoader::OnQuestAssetLoadComplete(FName QuestId, FOnQuestAssetLoadedNative OnLoaded)
{
	UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: OnQuestAssetLoadComplete called for quest '%s'"), *QuestId.ToString());
	
	// Remove from loading list
	TSharedPtr<FStreamableHandle> Handle;
	if (LoadingHandles.RemoveAndCopyValue(QuestId, Handle))
	{
		UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Found loading handle for quest '%s'"), *QuestId.ToString());
		
		if (Handle.IsValid() && Handle->HasLoadCompleted())
		{
			UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Handle is valid and load completed for quest '%s'"), *QuestId.ToString());
			
			UQuestAsset* LoadedAsset = Cast<UQuestAsset>(Handle->GetLoadedAsset());
			if (LoadedAsset)
			{
				// ?? DIAGNOSTIC: Log quest structure immediately after load
				UE_LOG(LogTemp, Warning, TEXT("?????? QUEST ASSET LOADED: '%s' (QuestId='%s')"), *LoadedAsset->GetName(), *LoadedAsset->QuestId.ToString());
				UE_LOG(LogTemp, Warning, TEXT("   Total Phases in asset: %d"), LoadedAsset->Phases.Num());
				
				for (int32 i = 0; i < LoadedAsset->Phases.Num(); i++)
				{
					const FQuestPhase& Phase = LoadedAsset->Phases[i];
					UE_LOG(LogTemp, Warning, TEXT("   Phase[%d]: '%s' (Title='%s')"), i, *Phase.PhaseId.ToString(), *Phase.Title.ToString());
					UE_LOG(LogTemp, Warning, TEXT("      PhaseRewards.Rewards.Num() = %d"), Phase.PhaseRewards.Rewards.Num());
					UE_LOG(LogTemp, Warning, TEXT("    PhaseRewards.GrantedTags.Num() = %d"), Phase.PhaseRewards.GrantedTags.Num());
					
					if (Phase.PhaseRewards.Rewards.Num() > 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("      ? Phase '%s' HAS REWARDS:"), *Phase.PhaseId.ToString());
						for (int32 j = 0; j < Phase.PhaseRewards.Rewards.Num(); j++)
						{
							const FQuestReward& Reward = Phase.PhaseRewards.Rewards[j];
							UE_LOG(LogTemp, Warning, TEXT("         Reward[%d]: Type=%d, Amount=%d, ItemId='%s'"), 
								j, (int32)Reward.RewardType, Reward.Amount, *Reward.ItemId.ToString());
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("      ?? Phase '%s' has NO rewards configured!"), *Phase.PhaseId.ToString());
					}
				}
				
				// Cache it
				LoadedQuests.Add(QuestId, LoadedAsset);
				UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Quest asset loaded successfully: '%s'"), *QuestId.ToString());
				OnLoaded.ExecuteIfBound(LoadedAsset);
				return;
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: Loaded asset is not a UQuestAsset for quest '%s'"), *QuestId.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: Handle is invalid or load not completed for quest '%s' (Valid=%d, Completed=%d)"), 
				*QuestId.ToString(), 
				Handle.IsValid() ? 1 : 0,
				Handle.IsValid() && Handle->HasLoadCompleted() ? 1 : 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: No loading handle found for quest '%s'"), *QuestId.ToString());
	}

	UE_LOG(LogTemp, Error, TEXT("QuestAssetLoader: Failed to load quest '%s'"), *QuestId.ToString());
	OnLoaded.ExecuteIfBound(nullptr);
}

void UQuestAssetLoader::PreloadQuestAssets(const TArray<FName>& QuestIds, FOnQuestAssetLoaded OnAllLoaded)
{
	if (QuestIds.Num() == 0)
	{
		OnAllLoaded.ExecuteIfBound(nullptr);
		return;
	}

	// Build list of asset paths
	TArray<FSoftObjectPath> AssetPaths;
	UAssetManager& AssetManager = UAssetManager::Get();

	for (FName QuestId : QuestIds)
	{
		// Skip if already loaded
		if (LoadedQuests.Contains(QuestId))
		{
			continue;
		}

		FPrimaryAssetId AssetId(FPrimaryAssetType("Quest"), QuestId);
		FAssetData AssetData;
		if (AssetManager.GetPrimaryAssetData(AssetId, AssetData))
		{
			AssetPaths.Add(AssetData.GetSoftObjectPath());
		}
	}

	if (AssetPaths.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: All quests already loaded"));
		OnAllLoaded.ExecuteIfBound(nullptr);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Preloading %d quest assets"), AssetPaths.Num());

	// Load all at once
	TSharedPtr<FStreamableHandle> Handle = GetStreamableManager().RequestAsyncLoad(
		AssetPaths,
		[this, QuestIds, OnAllLoaded]()
		{
			UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Preload complete"));
			
			// Cache all loaded assets
			UAssetManager& AssetManager = UAssetManager::Get();
			for (FName QuestId : QuestIds)
			{
				FPrimaryAssetId AssetId(FPrimaryAssetType("Quest"), QuestId);
				UQuestAsset* Asset = Cast<UQuestAsset>(AssetManager.GetPrimaryAssetObject(AssetId));
				if (Asset)
				{
					LoadedQuests.Add(QuestId, Asset);
				}
			}

			OnAllLoaded.ExecuteIfBound(nullptr);
		}
	);
}

void UQuestAssetLoader::CancelAllLoads()
{
	for (auto& Pair : LoadingHandles)
	{
		if (Pair.Value.IsValid())
		{
			Pair.Value->CancelHandle();
		}
	}
	LoadingHandles.Empty();

	UE_LOG(LogTemp, Log, TEXT("QuestAssetLoader: Cancelled all pending loads"));
}

UQuestAsset* UQuestAssetLoader::GetLoadedQuestAsset(FName QuestId) const
{
	if (const TObjectPtr<UQuestAsset>* Asset = LoadedQuests.Find(QuestId))
	{
		return Asset->Get();
	}
	return nullptr;
}

bool UQuestAssetLoader::IsQuestLoading(FName QuestId) const
{
	return LoadingHandles.Contains(QuestId);
}
