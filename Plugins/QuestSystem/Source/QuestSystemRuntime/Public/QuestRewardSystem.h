// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestTypes.h"
#include "QuestStructures.h"
#include "QuestRewardSystem.generated.h"

class UQuestSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRewardGrantedSignature, FName, QuestId, const FQuestReward&, Reward);

/**
 * Quest Reward System - Handles reward granting and integration with game systems
 * Single Responsibility: Reward processing and distribution
 */
UCLASS(BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestRewardSystem : public UObject
{
	GENERATED_BODY()

public:
	UQuestRewardSystem();

	/** Initialize with save game reference */
	void Initialize(UQuestSaveGame* InSaveGame);
	void Shutdown();

	// === Reward Processing ===

	/**
	 * Process all rewards in a reward set
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Rewards")
	void ProcessQuestRewards(const FQuestRewardSet& Rewards, FName QuestId);

	/**
	 * Grant single reward
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Rewards")
	void GrantReward(const FQuestReward& Reward, FName QuestId);

	/**
	 * Grant tags from reward set
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest|Rewards")
	void GrantTags(const TArray<FGameplayTag>& Tags);

	// === Integration Hooks ===
	// These should be overridden or bound to actual game systems

	/**
	 * Override this in Blueprint or bind delegate to integrate with inventory
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnGrantItem(FName ItemId, int32 Amount, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to integrate with economy
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnGrantCurrency(int32 Amount, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to integrate with stats
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnModifyStat(FGameplayTag StatTag, float Delta, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to integrate with affection system
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnModifyAffection(FName NpcId, float Delta, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to integrate with dialog system
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnUnlockDialogBranch(FName NpcId, FName DialogBranchId, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to integrate with crafting
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnUnlockRecipe(FName RecipeId, FName QuestId);

	/**
	 * Override this in Blueprint or bind delegate to execute custom blueprint events
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Quest|Rewards")
	void OnRunBlueprintEvent(FName EventName, FName QuestId);

	// === Events ===

	UPROPERTY(BlueprintAssignable, Category = "Quest|Rewards")
	FOnRewardGrantedSignature OnRewardGranted;

	// === Query ===

	/**
	 * Check if player has specific tag
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Rewards")
	bool HasTag(FName Tag) const;

	/**
	 * Get all granted tags
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest|Rewards")
	TArray<FName> GetAllTags() const;

protected:
	UPROPERTY()
	TObjectPtr<UQuestSaveGame> SaveGame;

	// Default implementations (log only)
	virtual void OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId);
	virtual void OnGrantCurrency_Implementation(int32 Amount, FName QuestId);
	virtual void OnModifyStat_Implementation(FGameplayTag StatTag, float Delta, FName QuestId);
	virtual void OnModifyAffection_Implementation(FName NpcId, float Delta, FName QuestId);
	virtual void OnUnlockDialogBranch_Implementation(FName NpcId, FName DialogBranchId, FName QuestId);
	virtual void OnUnlockRecipe_Implementation(FName RecipeId, FName QuestId);
	virtual void OnRunBlueprintEvent_Implementation(FName EventName, FName QuestId);
};
