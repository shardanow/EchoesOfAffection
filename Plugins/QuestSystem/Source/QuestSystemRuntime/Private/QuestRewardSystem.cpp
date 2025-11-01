// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestRewardSystem.h"
#include "QuestSaveGame.h"
#include "GameEventBusLibrary.h"

UQuestRewardSystem::UQuestRewardSystem()
{
}

void UQuestRewardSystem::Initialize(UQuestSaveGame* InSaveGame)
{
	SaveGame = InSaveGame;
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Initialized"));
}

void UQuestRewardSystem::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Shutdown"));
}

// === Reward Processing ===

void UQuestRewardSystem::ProcessQuestRewards(const FQuestRewardSet& Rewards, FName QuestId)
{
	UE_LOG(LogTemp, Warning, TEXT("?????? QuestRewardSystem::ProcessQuestRewards called for Quest '%s'"), *QuestId.ToString());
	UE_LOG(LogTemp, Warning, TEXT("   - Rewards count: %d"), Rewards.Rewards.Num());
	UE_LOG(LogTemp, Warning, TEXT("   - GrantedTags count: %d"), Rewards.GrantedTags.Num());

	if (Rewards.Rewards.Num() == 0 && Rewards.GrantedTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("?? No rewards to grant! RewardsOnComplete is empty in Quest Asset."));
		return;
	}

	// Process rewards
	for (const FQuestReward& Reward : Rewards.Rewards)
	{
		GrantReward(Reward, QuestId);
	}
	
	// Grant tags
	if (Rewards.GrantedTags.Num() > 0)
	{
		GrantTags(Rewards.GrantedTags.GetGameplayTagArray());
	}
}

void UQuestRewardSystem::GrantReward(const FQuestReward& Reward, FName QuestId)
{
	UE_LOG(LogTemp, Warning, TEXT("   ?? Granting reward: Type=%d, for Quest '%s'"), (int32)Reward.RewardType, *QuestId.ToString());

	switch (Reward.RewardType)
	{
	case EQuestRewardType::GiveItem:
		OnGrantItem(Reward.ItemId, Reward.Amount, QuestId);
		break;

	case EQuestRewardType::GrantCurrency:
		OnGrantCurrency(Reward.CurrencyAmount, QuestId);
		break;

	case EQuestRewardType::ModifyStats:
	case EQuestRewardType::ModifyNeeds:
		OnModifyStat(Reward.StatTag, Reward.StatDelta, QuestId);
		break;

	case EQuestRewardType::ModifyAffection:
		OnModifyAffection(Reward.NpcId, Reward.AffectionDelta, QuestId);
		break;

	case EQuestRewardType::UnlockDialogBranch:
		OnUnlockDialogBranch(Reward.NpcId, Reward.DialogBranchId, QuestId);
		break;

	case EQuestRewardType::UnlockRecipe:
		OnUnlockRecipe(Reward.RecipeId, QuestId);
		break;

	case EQuestRewardType::GrantTag:
		{
			TArray<FGameplayTag> SingleTag;
			SingleTag.Add(Reward.GrantedTag);
			GrantTags(SingleTag);
		}
		break;

	case EQuestRewardType::RunBPEvent:
		OnRunBlueprintEvent(Reward.BPEventName, QuestId);
		break;

	case EQuestRewardType::ExecuteEffect:
		// Custom effect execution - to be implemented by game
		UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Execute custom effect for quest '%s'"), *QuestId.ToString());
		break;
	}

	// Broadcast event
	OnRewardGranted.Broadcast(QuestId, Reward);
}

void UQuestRewardSystem::GrantTags(const TArray<FGameplayTag>& Tags)
{
	if (!SaveGame)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestRewardSystem: Cannot grant tags - no save game set"));
		return;
	}

	for (const FGameplayTag& Tag : Tags)
	{
		FName TagName = FName(*Tag.ToString());
		if (!SaveGame->GlobalQuestTags.Contains(TagName))
		{
			SaveGame->GlobalQuestTags.Add(TagName);
			UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Granted tag '%s'"), *TagName.ToString());
		}
	}
}

// === Query ===

bool UQuestRewardSystem::HasTag(FName Tag) const
{
	if (!SaveGame)
	{
		return false;
	}
	return SaveGame->GlobalQuestTags.Contains(Tag);
}

TArray<FName> UQuestRewardSystem::GetAllTags() const
{
	if (!SaveGame)
	{
		return TArray<FName>();
	}
	return SaveGame->GlobalQuestTags.Array();  // Convert TSet to TArray
}

// === Default Implementations (Log Only) ===
// Override these in Blueprint or bind to actual game systems

void UQuestRewardSystem::OnGrantItem_Implementation(FName ItemId, int32 Amount, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Grant item '%s' x%d (Quest: %s)"), 
		*ItemId.ToString(), Amount, *QuestId.ToString());
	
	// Emit GameEvent instead of directly calling InventoryComponent
	// Event: GameEvent.Quest.GrantItem
	// Params: ItemId (StringParam), Amount (IntParam), QuestId (source context)
	// 
	// This event will be handled by InventorySystemGameEventBridge which will:
	// 1. Receive the event
	// 2. Load ItemDataAsset
	// 3. Call InventoryComponent->AddItem()
	
	FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("GameEvent.Quest.GrantItem"));
	if (!EventTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("QuestRewardSystem: EventTag 'GameEvent.Quest.GrantItem' is INVALID!"));
		UE_LOG(LogTemp, Error, TEXT("   Please add to Config/DefaultGameplayTags.ini:"));
		UE_LOG(LogTemp, Error, TEXT("   +GameplayTagList=(Tag=\"GameEvent.Quest.GrantItem\",DevComment=\"[QuestSystem] Request to grant item to player\")"));
		return;
	}

	// Use helper function from GameEventBusLibrary
	UGameEventBusLibrary::EmitItemEvent(
		this, // WorldContextObject
		EventTag,         // EventTag
		ItemId,              // ItemId
		Amount,     // Count
		nullptr,  // Instigator (not needed)
		true   // bLogEvent
	);

	UE_LOG(LogTemp, Warning, TEXT("? QuestRewardSystem: Emitted 'GameEvent.Quest.GrantItem' for ItemId='%s', Amount=%d, QuestId='%s'"), 
		*ItemId.ToString(), Amount, *QuestId.ToString());
}

void UQuestRewardSystem::OnGrantCurrency_Implementation(int32 Amount, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Grant currency %d (Quest: %s)"), 
		Amount, *QuestId.ToString());
	
	// TODO: Integrate with economy system
	// Example:
	// UEconomySystem* Economy = GetEconomySystem();
	// Economy->AddCurrency(Amount);
}

void UQuestRewardSystem::OnModifyStat_Implementation(FGameplayTag StatTag, float Delta, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Modify stat '%s' by %f (Quest: %s)"), 
		*StatTag.ToString(), Delta, *QuestId.ToString());
	
	// TODO: Integrate with stats/attribute system
	// Example:
	// UAttributeComponent* Attributes = GetPlayerAttributes();
	// Attributes->ModifyStat(StatTag, Delta);
}

void UQuestRewardSystem::OnModifyAffection_Implementation(FName NpcId, float Delta, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Modify affection with '%s' by %f (Quest: %s)"), 
		*NpcId.ToString(), Delta, *QuestId.ToString());
	
	// TODO: Integrate with affection/relationship system
	// Example:
	// UAffectionSystem* Affection = GetAffectionSystem();
	// Affection->ModifyAffection(NpcId, Delta);
}

void UQuestRewardSystem::OnUnlockDialogBranch_Implementation(FName NpcId, FName DialogBranchId, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Unlock dialog branch '%s' for NPC '%s' (Quest: %s)"), 
		*DialogBranchId.ToString(), *NpcId.ToString(), *QuestId.ToString());
	
	// TODO: Integrate with dialog system
	// Example:
	// UDialogSystem* DialogSys = GetDialogSystem();
	// DialogSys->UnlockBranch(NpcId, DialogBranchId);
}

void UQuestRewardSystem::OnUnlockRecipe_Implementation(FName RecipeId, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Unlock recipe '%s' (Quest: %s)"), 
		*RecipeId.ToString(), *QuestId.ToString());
	
	// TODO: Integrate with crafting system
	// Example:
	// UCraftingSystem* Crafting = GetCraftingSystem();
	// Crafting->UnlockRecipe(RecipeId);
}

void UQuestRewardSystem::OnRunBlueprintEvent_Implementation(FName EventName, FName QuestId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestRewardSystem: Run blueprint event '%s' (Quest: %s)"), 
		*EventName.ToString(), *QuestId.ToString());
	
	// This is meant to be overridden in Blueprint
	// Example Blueprint implementation:
	// - Check EventName
	// - Execute corresponding custom logic
	// - Spawn particles, play sounds, etc.
}
