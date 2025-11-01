// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestSubsystem.h"
#include "QuestProgressManager.h"
#include "QuestRewardSystem.h"
#include "QuestAssetLoader.h"
#include "QuestEventBus.h"
#include "QuestSaveManager.h"
#include "QuestSaveGame.h"

UQuestSubsystem::UQuestSubsystem()
{
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem v2.0: Initializing..."));
	
	InitializeSubsystems();
	ConnectSubsystems();
	LoadInitialData();
	InitializeGameEventBridge();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem v2.0: Initialized successfully"));
}

void UQuestSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem v2.0: Deinitializing..."));
	
	// Shutdown GameEventBridge if it exists
	if (GameEventBridge)
	{
		// Call Shutdown via reflection to avoid hard dependency
		UFunction* ShutdownFunc = GameEventBridge->GetClass()->FindFunctionByName(TEXT("Shutdown"));
		if (ShutdownFunc)
		{
			GameEventBridge->ProcessEvent(ShutdownFunc, nullptr);
		}
		GameEventBridge = nullptr;
	}
	
	if (SaveManager && CurrentSaveGame)
	{
		SaveManager->SaveQuestProgress(CurrentSaveGame);
	}
	
	if (ProgressManager)
	{
		ProgressManager->Shutdown();
	}
	
	if (RewardSystem)
	{
		RewardSystem->Shutdown();
	}
	
	if (EventBus)
	{
		EventBus->ClearAllSubscriptions();
	}
	
	if (SaveManager)
	{
		SaveManager->Shutdown();
	}
	
	Super::Deinitialize();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem v2.0: Deinitialized"));
}

void UQuestSubsystem::InitializeSubsystems()
{
	AssetLoader = NewObject<UQuestAssetLoader>(this, TEXT("AssetLoader"));
	EventBus = NewObject<UQuestEventBus>(this, TEXT("EventBus"));
	ProgressManager = NewObject<UQuestProgressManager>(this, TEXT("ProgressManager"));
	RewardSystem = NewObject<UQuestRewardSystem>(this, TEXT("RewardSystem"));
	SaveManager = NewObject<UQuestSaveManager>(this, TEXT("SaveManager"));
	
	SaveManager->Initialize();
	
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Sub-systems created"));
}

void UQuestSubsystem::ConnectSubsystems()
{
	CurrentSaveGame = NewObject<UQuestSaveGame>(this, TEXT("QuestSaveData"));
	
	ProgressManager->Initialize(EventBus, AssetLoader);
	ProgressManager->SetSaveGame(CurrentSaveGame);
	
	// Initialize EventBus with ProgressManager for automatic quest updates
	EventBus->Initialize(ProgressManager);
	
	RewardSystem->Initialize(CurrentSaveGame);
	
	ProgressManager->OnQuestCompleted.AddDynamic(this, &UQuestSubsystem::OnQuestCompletedInternal);
	ProgressManager->OnObjectiveCompleted.AddDynamic(this, &UQuestSubsystem::OnObjectiveCompletedInternal);
	ProgressManager->OnQuestPhaseChanged.AddDynamic(this, &UQuestSubsystem::OnPhaseChangedInternal);
	
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Sub-systems connected"));
}

void UQuestSubsystem::LoadInitialData()
{
	UQuestSaveGame* LoadedSave = SaveManager->LoadQuestProgress(
		SaveManager->DefaultSlotName,
		SaveManager->DefaultUserIndex
	);
	
	if (LoadedSave)
	{
		CurrentSaveGame = LoadedSave;
		ProgressManager->SetSaveGame(CurrentSaveGame);
		RewardSystem->Initialize(CurrentSaveGame);
		
		UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Loaded %d quests from save"), 
			CurrentSaveGame->QuestProgress.Num());
		
		// Preload quest assets for active quests
		TArray<FQuestSaveData> ActiveQuests = CurrentSaveGame->GetQuestsByState(EQuestState::Active);
		for (const FQuestSaveData& QuestData : ActiveQuests)
		{
			if (!QuestData.QuestId.IsNone())
			{
				UE_LOG(LogTemp, Log, TEXT("QuestSubsystem: Preloading asset for active quest '%s'"), *QuestData.QuestId.ToString());
				
				// Load asynchronously (no callback needed - will be cached)
				FOnQuestAssetLoaded EmptyCallback;
				AssetLoader->LoadQuestAssetAsync(QuestData.QuestId, EmptyCallback);
			}
		}
	}
}

void UQuestSubsystem::OnQuestCompletedInternal(FName QuestId, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Warning, TEXT("?????? QuestSubsystem::OnQuestCompletedInternal - Quest '%s' completed!"), *QuestId.ToString());

	if (QuestAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("   ? QuestAsset is valid"));
		UE_LOG(LogTemp, Warning, TEXT("   ?? Quest Title: '%s'"), *QuestAsset->Title.ToString());
		UE_LOG(LogTemp, Warning, TEXT("   ?? RewardsOnComplete.Rewards count: %d"), QuestAsset->RewardsOnComplete.Rewards.Num());
		UE_LOG(LogTemp, Warning, TEXT("   ??? RewardsOnComplete.GrantedTags count: %d"), QuestAsset->RewardsOnComplete.GrantedTags.Num());
		
		// Log each reward
		for (int32 i = 0; i < QuestAsset->RewardsOnComplete.Rewards.Num(); i++)
		{
			const FQuestReward& Reward = QuestAsset->RewardsOnComplete.Rewards[i];
			UE_LOG(LogTemp, Warning, TEXT("   Reward[%d]: Type=%d, Amount=%d, ItemId='%s'"), 
				i, (int32)Reward.RewardType, Reward.Amount, *Reward.ItemId.ToString());
		}
		
		RewardSystem->ProcessQuestRewards(QuestAsset->RewardsOnComplete, QuestId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("   ? QuestAsset is NULL! Cannot process rewards."));
	}
}

void UQuestSubsystem::OnObjectiveCompletedInternal(FName QuestId, FName ObjectiveId)
{
	UE_LOG(LogTemp, Warning, TEXT("? Objective '%s' completed in quest '%s'"), *ObjectiveId.ToString(), *QuestId.ToString());
	
	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (QuestAsset)
	{
		FQuestPhase Phase = QuestAsset->GetPhase(ProgressManager->GetCurrentPhaseId(QuestId));
		for (const FQuestObjective& Obj : Phase.Objectives)
		{
			if (Obj.ObjectiveId == ObjectiveId)
			{
				// Process objective-specific rewards (NOT phase rewards!)
				// Phase rewards are processed in OnPhaseChangedInternal()
				if (Obj.Signals.OnCompleteRewards.Rewards.Num() > 0 || Obj.Signals.OnCompleteRewards.GrantedTags.Num() > 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("?? Processing objective-specific rewards for '%s'"), *ObjectiveId.ToString());
					RewardSystem->ProcessQuestRewards(Obj.Signals.OnCompleteRewards, QuestId);
				}
				break;
			}
		}
	}
}

void UQuestSubsystem::OnPhaseChangedInternal(FName QuestId, FName OldPhaseId, FName NewPhaseId)
{
	UE_LOG(LogTemp, Warning, TEXT("?? QuestSubsystem::OnPhaseChangedInternal - Quest '%s': '%s' ? '%s'"), 
		*QuestId.ToString(), *OldPhaseId.ToString(), *NewPhaseId.ToString());

	// Process phase completion rewards for OLD phase
	if (!OldPhaseId.IsNone())
	{
		UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
		if (QuestAsset)
		{
			FQuestPhase OldPhase = QuestAsset->GetPhase(OldPhaseId);
			
			// ?? DIAGNOSTIC: Always log phase rewards status
			UE_LOG(LogTemp, Warning, TEXT("?? Phase '%s' rewards check:"), *OldPhaseId.ToString());
			UE_LOG(LogTemp, Warning, TEXT("   - PhaseRewards.Rewards count: %d"), OldPhase.PhaseRewards.Rewards.Num());
			UE_LOG(LogTemp, Warning, TEXT("   - PhaseRewards.GrantedTags count: %d"), OldPhase.PhaseRewards.GrantedTags.Num());
			
			// Log individual rewards for debugging
			for (int32 i = 0; i < OldPhase.PhaseRewards.Rewards.Num(); i++)
			{
				const FQuestReward& Reward = OldPhase.PhaseRewards.Rewards[i];
				UE_LOG(LogTemp, Warning, TEXT("   Reward[%d]: Type=%d, Amount=%d, ItemId='%s'"), 
					i, (int32)Reward.RewardType, Reward.Amount, *Reward.ItemId.ToString());
			}
			
			if (OldPhase.PhaseRewards.Rewards.Num() > 0 || OldPhase.PhaseRewards.GrantedTags.Num() > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("?? Processing phase completion rewards for '%s'"), *OldPhaseId.ToString());
				RewardSystem->ProcessQuestRewards(OldPhase.PhaseRewards, QuestId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("?? Phase '%s' has NO rewards configured!"), *OldPhaseId.ToString());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("? QuestAsset not found for quest '%s'"), *QuestId.ToString());
		}
	}
}

void UQuestSubsystem::StartQuestAsync(FName QuestId, FOnQuestStartedDelegate OnStarted)
{
	if (!ProgressManager)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestSubsystem: ProgressManager not initialized"));
		OnStarted.ExecuteIfBound(false, nullptr);
		return;
	}
	
	// Store the callback for later use
	PendingQuestCallbacks.Add(QuestId, OnStarted);
	
	// Create callback that will be called by ProgressManager
	FOnQuestStartedCallback Callback;
	Callback.BindDynamic(this, &UQuestSubsystem::OnQuestStartedInternal);
	
	ProgressManager->StartQuestAsync(QuestId, Callback);
}

void UQuestSubsystem::OnQuestStartedInternal(bool bSuccess, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::OnQuestStartedInternal called - bSuccess=%s, QuestAsset=%s"), 
		bSuccess ? TEXT("TRUE") : TEXT("FALSE"), 
		QuestAsset ? *QuestAsset->GetName() : TEXT("NULL"));
	
	if (!QuestAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestSubsystem::OnQuestStartedInternal - QuestAsset is NULL"));
		return;
	}
	
	FName QuestId = QuestAsset->QuestId;
	UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::OnQuestStartedInternal - Processing quest '%s'"), *QuestId.ToString());
	
	// Find and execute the pending callback
	if (FOnQuestStartedDelegate* PendingCallback = PendingQuestCallbacks.Find(QuestId))
	{
		UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::OnQuestStartedInternal - Found pending callback for quest '%s', executing..."), *QuestId.ToString());
		PendingCallback->ExecuteIfBound(bSuccess, QuestAsset);
		PendingQuestCallbacks.Remove(QuestId);
		UE_LOG(LogTemp, Log, TEXT("QuestSubsystem::OnQuestStartedInternal - Callback executed and removed for quest '%s'"), *QuestId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestSubsystem::OnQuestStartedInternal - No pending callback found for quest '%s'"), *QuestId.ToString());
	}
}

bool UQuestSubsystem::CompleteQuest(FName QuestId)
{
	return ProgressManager ? ProgressManager->CompleteQuest(QuestId) : false;
}

bool UQuestSubsystem::UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount)
{
	return ProgressManager ? ProgressManager->UpdateObjectiveProgress(QuestId, ObjectiveId, Amount) : false;
}

void UQuestSubsystem::GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const
{
	if (ProgressManager)
	{
		ProgressManager->GetObjectiveProgress(QuestId, ObjectiveId, CurrentCount, TargetCount, bIsCompleted);
	}
	else
	{
		CurrentCount = 0;
		TargetCount = 1;
		bIsCompleted = false;
	}
}

bool UQuestSubsystem::IsQuestActive(FName QuestId) const
{
	return ProgressManager ? ProgressManager->IsQuestActive(QuestId) : false;
}

TArray<FQuestSaveData> UQuestSubsystem::GetActiveQuests() const
{
	return ProgressManager ? ProgressManager->GetActiveQuests() : TArray<FQuestSaveData>();
}

TArray<FQuestSaveData> UQuestSubsystem::GetCompletedQuests() const
{
	return ProgressManager ? ProgressManager->GetCompletedQuests() : TArray<FQuestSaveData>();
}

UQuestAsset* UQuestSubsystem::GetQuestAsset(FName QuestId) const
{
	return AssetLoader ? AssetLoader->GetLoadedQuestAsset(QuestId) : nullptr;
}

EQuestState UQuestSubsystem::GetQuestState(FName QuestId) const
{
	return ProgressManager ? ProgressManager->GetQuestState(QuestId) : EQuestState::NotStarted;
}

FQuestSaveData UQuestSubsystem::GetQuestSaveData(FName QuestId) const
{
	return ProgressManager ? ProgressManager->GetQuestSaveData(QuestId) : FQuestSaveData();
}

bool UQuestSubsystem::CanStartQuest(FName QuestId, TArray<FText>& OutFailReasons) const
{
	return ProgressManager ? ProgressManager->CanStartQuest(QuestId, OutFailReasons) : false;
}

void UQuestSubsystem::EmitQuestEvent(const FQuestEventPayload& Payload)
{
	if (EventBus)
	{
		EventBus->EmitEvent(Payload);
	}
}

void UQuestSubsystem::ProcessPhaseRewards(FName QuestId, FName PhaseId, const FQuestRewardSet& Rewards)
{
	UE_LOG(LogTemp, Warning, TEXT("?? QuestSubsystem::ProcessPhaseRewards - Quest '%s', Phase '%s'"), *QuestId.ToString(), *PhaseId.ToString());
	
	if (RewardSystem)
	{
		RewardSystem->ProcessQuestRewards(Rewards, QuestId);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("? RewardSystem is NULL! Cannot process phase rewards."));
	}
}

bool UQuestSubsystem::IsQuestCompleted(FName QuestId) const
{
	return ProgressManager ? ProgressManager->IsQuestCompleted(QuestId) : false;
}

bool UQuestSubsystem::FailQuest(FName QuestId)
{
	return ProgressManager ? ProgressManager->FailQuest(QuestId) : false;
}

bool UQuestSubsystem::AbandonQuest(FName QuestId)
{
	return ProgressManager ? ProgressManager->AbandonQuest(QuestId) : false;
}

bool UQuestSubsystem::CompleteObjective(FName QuestId, FName ObjectiveId)
{
	return ProgressManager ? ProgressManager->CompleteObjective(QuestId, ObjectiveId) : false;
}

FName UQuestSubsystem::GetCurrentPhaseId(FName QuestId) const
{
	return ProgressManager ? ProgressManager->GetCurrentPhaseId(QuestId) : NAME_None;
}

void UQuestSubsystem::SaveAsync()
{
	if (!SaveManager || !CurrentSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestSubsystem: Cannot save - SaveManager or SaveGame is null"));
		return;
	}
	
	FOnSaveCompleteCallback SaveCallback;
	SaveManager->SaveQuestProgressAsync(CurrentSaveGame, SaveCallback);
}

void UQuestSubsystem::LoadAsync()
{
	if (!SaveManager)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestSubsystem: Cannot load - SaveManager is null"));
		return;
	}
	
	FOnLoadCompleteCallback LoadCallback;
	SaveManager->LoadQuestProgressAsync(
		SaveManager->DefaultSlotName,
		SaveManager->DefaultUserIndex,
		LoadCallback
	);
}

void UQuestSubsystem::EnableAutoSave(float IntervalSeconds)
{
	if (SaveManager)
	{
		SaveManager->EnableAutoSave(IntervalSeconds);
	}
}

UQuestSubsystem* UQuestSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}
	
	return GameInstance->GetSubsystem<UQuestSubsystem>();
}

void UQuestSubsystem::InitializeGameEventBridge()
{
	UE_LOG(LogTemp, Log, TEXT("?? InitializeGameEventBridge: Starting..."));

	// Try to find QuestEventBridge class (may not exist if GameEventBus plugin not enabled)
	UClass* BridgeClass = FindObject<UClass>(ANY_PACKAGE, TEXT("QuestEventBridge"));
	if (!BridgeClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("?? QuestEventBridge class not found via FindObject, trying StaticLoadClass..."));
		// Try loading the class
		BridgeClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/QuestSystemRuntime.QuestEventBridge"));
		
		if (BridgeClass)
		{
			UE_LOG(LogTemp, Log, TEXT("? QuestEventBridge class loaded via StaticLoadClass!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("? QuestEventBridge class NOT FOUND! GameEventBus integration will NOT work!"));
			UE_LOG(LogTemp, Error, TEXT("   Make sure QuestEventBridge.h/cpp are included in the plugin build."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("? QuestEventBridge class found via FindObject!"));
	}

	if (BridgeClass)
	{
		UE_LOG(LogTemp, Log, TEXT("?? Creating QuestEventBridge instance..."));
		GameEventBridge = NewObject<UObject>(this, BridgeClass, TEXT("GameEventBridge"));
		
		if (!GameEventBridge)
		{
			UE_LOG(LogTemp, Error, TEXT("? Failed to create GameEventBridge instance!"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("? GameEventBridge instance created!"));
		
		// Call Initialize via reflection (to avoid hard dependency)
		UFunction* InitFunc = BridgeClass->FindFunctionByName(TEXT("Initialize"));
		if (InitFunc && GameEventBridge)
		{
			UE_LOG(LogTemp, Log, TEXT("?? Calling GameEventBridge::Initialize..."));

			struct FInitParams
			{
				UObject* QuestSubsystem;
			};

			FInitParams Params;
			Params.QuestSubsystem = this;

			GameEventBridge->ProcessEvent(InitFunc, &Params);
			
			UE_LOG(LogTemp, Log, TEXT("? QuestSubsystem: GameEventBridge initialized successfully!"));
		}
		else
		{
			if (!InitFunc)
			{
				UE_LOG(LogTemp, Error, TEXT("? GameEventBridge::Initialize function not found!"));
			}
			if (!GameEventBridge)
			{
				UE_LOG(LogTemp, Error, TEXT("? GameEventBridge instance is NULL!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("?? QuestEventBridge class not available - quests will not react to game events"));
		UE_LOG(LogTemp, Warning, TEXT("   This is normal if GameEventBus plugin is disabled"));
	}
}

// === DIAGNOSTIC COMMANDS ===

void UQuestSubsystem::QuestDebugPrint()
{
	if (!ProgressManager)
	{
		UE_LOG(LogTemp, Error, TEXT("=== QUEST DEBUG: ProgressManager is NULL ==="));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));
	UE_LOG(LogTemp, Log, TEXT("?         QUEST DEBUG SUMMARY   ?"));
	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));

	TArray<FQuestSaveData> ActiveQuests = ProgressManager->GetActiveQuests();
	UE_LOG(LogTemp, Log, TEXT("?? Active Quests: %d"), ActiveQuests.Num());

	if (ActiveQuests.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("  ?? NO ACTIVE QUESTS!"));
		return;
	}

	for (const FQuestSaveData& QuestData : ActiveQuests)
	{
		UE_LOG(LogTemp, Log, TEXT("???????????????????????????????????????????????????????????"));
		UE_LOG(LogTemp, Log, TEXT("?? Quest: '%s'"), *QuestData.QuestId.ToString());
		UE_LOG(LogTemp, Log, TEXT("   State: %d"), (int32)QuestData.State);
		UE_LOG(LogTemp, Log, TEXT("   Current Phase: '%s'"), *QuestData.CurrentPhaseId.ToString());

		UQuestAsset* QuestAsset = GetQuestAsset(QuestData.QuestId);
		if (!QuestAsset)
		{
			UE_LOG(LogTemp, Error, TEXT("   ? Quest Asset NOT FOUND!"));
			continue;
		}

		// Find current phase
		const FQuestPhase* CurrentPhase = nullptr;
		for (const FQuestPhase& Phase : QuestAsset->Phases)
		{
			if (Phase.PhaseId == QuestData.CurrentPhaseId)
			{
				CurrentPhase = &Phase;
				break;
			}
		}

		if (!CurrentPhase)
		{
			UE_LOG(LogTemp, Error, TEXT("   ? Current Phase NOT FOUND in asset!"));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("   ?? Objectives (%d):"), CurrentPhase->Objectives.Num());
		for (const FQuestObjective& Objective : CurrentPhase->Objectives)
		{
			int32 CurrentCount, TargetCount;
			bool bIsCompleted;
			GetObjectiveProgress(QuestData.QuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

			FString Status = bIsCompleted ? TEXT("? COMPLETED") : TEXT("? IN PROGRESS");
			UE_LOG(LogTemp, Log, TEXT("    %s '%s': %d/%d"), *Status, *Objective.ObjectiveId.ToString(), CurrentCount, TargetCount);

			// Print conditions
			UE_LOG(LogTemp, Log, TEXT(" Conditions (%d):"), Objective.Conditions.Num());
			for (const FObjectiveCondition& Condition : Objective.Conditions)
			{
				UE_LOG(LogTemp, Log, TEXT("        - EventTag='%s', ItemId='%s', NpcId='%s', Count=%d"),
					*Condition.EventTag.ToString(),
					*Condition.ItemId.ToString(),
					*Condition.NpcId.ToString(),
					Condition.Count);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));
}

void UQuestSubsystem::QuestDebugListeners()
{
	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));
	UE_LOG(LogTemp, Log, TEXT("?               QUEST EVENT LISTENERS             ?"));
	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));

	if (!EventBus)
	{
		UE_LOG(LogTemp, Error, TEXT("? EventBus is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("? QuestEventBus is active"));
	UE_LOG(LogTemp, Log, TEXT("? ProgressManager reference: %s"), ProgressManager ? TEXT("VALID") : TEXT("NULL"));

	if (GameEventBridge)
	{
		UE_LOG(LogTemp, Log, TEXT("? GameEventBridge is CONNECTED"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("?? GameEventBridge is NULL (GameEventBus plugin may not be enabled)"));
	}

	UE_LOG(LogTemp, Log, TEXT("?????????????????????????????????????????????????????????????"));
}

void UQuestSubsystem::QuestTestEvent(const FString& EventTag, const FString& StringParam, int32 IntParam)
{
	UE_LOG(LogTemp, Warning, TEXT("?????????????????????????????????????????????????????????????"));
	UE_LOG(LogTemp, Warning, TEXT("?     TEST EVENT EMISSION  ?"));
	UE_LOG(LogTemp, Warning, TEXT("?????????????????????????????????????????????????????????????"));
	UE_LOG(LogTemp, Warning, TEXT("?? Emitting test event:"));
	UE_LOG(LogTemp, Warning, TEXT("   EventTag: '%s'"), *EventTag);
	UE_LOG(LogTemp, Warning, TEXT("StringParam: '%s'"), *StringParam);
	UE_LOG(LogTemp, Warning, TEXT("   IntParam: %d"), IntParam);

	FQuestEventPayload Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName(*EventTag));
	Payload.StringParam = FName(*StringParam);
	Payload.IntParam = IntParam;

	EmitQuestEvent(Payload);

	UE_LOG(LogTemp, Warning, TEXT("? Test event emitted! Check logs above for processing details."));
	UE_LOG(LogTemp, Warning, TEXT("?????????????????????????????????????????????????????????????"));
}
