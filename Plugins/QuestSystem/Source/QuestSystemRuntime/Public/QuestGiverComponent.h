// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestTypes.h"
#include "QuestAsset.h"
#include "QuestLogComponent.h"
#include "QuestGiverComponent.generated.h"

// Forward declarations
class UQuestAsset;
class UQuestSubsystem;

/**
 * Quest Giver Component - Attach to NPCs or objects that give quests
 * 
 * Usage:
 * 1. Add this component to NPC Blueprint
 * 2. Configure available quests in Details panel
 * 3. Call GetAvailableQuests() to show quest UI
 * 4. Call AcceptQuest() when player accepts
 */
UCLASS(ClassGroup=(Quest), meta=(BlueprintSpawnableComponent), BlueprintType)
class QUESTSYSTEMRUNTIME_API UQuestGiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestGiverComponent();

	//~ Begin UActorComponent Interface
	virtual void BeginPlay() override;
	//~ End UActorComponent Interface

	/** Quests this NPC can give */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver")
	TArray<TSoftObjectPtr<UQuestAsset>> AvailableQuests;

	/** NPC identifier for quest conditions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver")
	FName NpcId;

	/** Auto-assign NPC ID from actor name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver")
	bool bAutoAssignNpcId = true;

	/** Icon to show when quests are available */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver|UI")
	UTexture2D* QuestAvailableIcon;

	/** Icon to show when quest is in progress */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver|UI")
	UTexture2D* QuestInProgressIcon;

	/** Icon to show when quest is completable */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Giver|UI")
	UTexture2D* QuestCompleteIcon;

	/**
	 * Get quests available to start
	 * Filters by dependencies and already completed quests
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	TArray<UQuestAsset*> GetAvailableQuestsForPlayer(APlayerController* Player) const;

	/**
	 * Get active quests from this NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	TArray<UQuestAsset*> GetActiveQuestsForPlayer(APlayerController* Player) const;

	/**
	 * Get completable quests (turn-in)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	TArray<UQuestAsset*> GetCompletableQuestsForPlayer(APlayerController* Player) const;

	/**
	 * Accept quest from this NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	bool AcceptQuest(APlayerController* Player, UQuestAsset* QuestAsset);

	/**
	 * Turn in completed quest
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	bool TurnInQuest(APlayerController* Player, UQuestAsset* QuestAsset);

	/**
	 * Check if this NPC has any interaction available
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Giver")
	bool HasQuestInteraction(APlayerController* Player) const;

	/**
	 * Get current quest indicator icon
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Quest Giver")
	UTexture2D* GetQuestIndicatorIcon(APlayerController* Player) const;

	/** Called when player interacts with this quest giver */
	UFUNCTION(BlueprintCallable, Category = "Quest Giver")
	void OnPlayerInteract(APlayerController* Player);

	/**
	 * Get available quests for actor (supports both Player and NPC)
	 * @param Actor - Interacting actor (Player Pawn, NPC, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<UQuestAsset*> GetAvailableQuestsForActor(AActor* Actor) const;

	/**
	 * Accept quest on behalf of actor
	 * @param QuestAsset - Quest to accept
	 * @param Actor - Actor accepting the quest (Player Pawn, NPC, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	bool AcceptQuestForActor(UQuestAsset* QuestAsset, AActor* Actor);

	/**
	 * Handle interaction with quest giver
	 * Called when actor interacts with this NPC
	 */
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void OnActorInteract(AActor* InteractingActor);

	// Delegates

	/** Broadcast when available quests change */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAvailableQuestsChanged);
	UPROPERTY(BlueprintAssignable, Category = "Quest Giver")
	FOnAvailableQuestsChanged OnAvailableQuestsChanged;

	/** Broadcast when quest is accepted */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestAccepted, UQuestAsset*, QuestAsset, APlayerController*, Player);
	UPROPERTY(BlueprintAssignable, Category = "Quest Giver")
	FOnQuestAccepted OnQuestAccepted;

	/** Broadcast when quest is turned in */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestTurnedIn, UQuestAsset*, QuestAsset, APlayerController*, Player);
	UPROPERTY(BlueprintAssignable, Category = "Quest Giver")
	FOnQuestTurnedIn OnQuestTurnedIn;

protected:
	/** Cache of loaded quest assets */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UQuestAsset>> LoadedQuests;

	/** Load quest assets */
	void LoadQuestAssets();

	/** Called when quest assets finish loading */
	void OnQuestsLoaded();

	/** Get quest subsystem */
	class UQuestSubsystem* GetQuestSubsystem() const;

	/**
	 * Get Quest Log Component from actor (Player or NPC)
	 */
	UQuestLogComponent* GetQuestLogFromActor(AActor* Actor) const;

	/**
	 * Get Player Controller from actor if possible
	 */
	APlayerController* GetPlayerControllerFromActor(AActor* Actor) const;
};
