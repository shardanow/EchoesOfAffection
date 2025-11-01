// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestGiverComponent.h"
#include "QuestSubsystem.h"
#include "QuestProgressManager.h"
#include "QuestEventBus.h"
#include "QuestAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerController.h"

UQuestGiverComponent::UQuestGiverComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoAssignNpcId = true;
}

void UQuestGiverComponent::BeginPlay()
{
	Super::BeginPlay();

	// Auto-assign NPC ID from actor name
	if (bAutoAssignNpcId && NpcId.IsNone())
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			NpcId = FName(*Owner->GetName());
		}
	}

	// Load quest assets
	LoadQuestAssets();

	UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Initialized on '%s' with NPC ID '%s'"), 
		*GetOwner()->GetName(), *NpcId.ToString());
}

void UQuestGiverComponent::LoadQuestAssets()
{
	if (AvailableQuests.Num() == 0)
	{
		return;
	}

	// Load all quest assets asynchronously
	TArray<FSoftObjectPath> AssetsToLoad;
	for (const TSoftObjectPtr<UQuestAsset>& QuestPtr : AvailableQuests)
	{
		if (!QuestPtr.IsNull())
		{
			AssetsToLoad.Add(QuestPtr.ToSoftObjectPath());
		}
	}

	if (AssetsToLoad.Num() > 0)
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();

		StreamableManager.RequestAsyncLoad(
			AssetsToLoad,
			FStreamableDelegate::CreateUObject(this, &UQuestGiverComponent::OnQuestsLoaded)
		);
	}
}

void UQuestGiverComponent::OnQuestsLoaded()
{
	LoadedQuests.Empty();

	for (const TSoftObjectPtr<UQuestAsset>& QuestPtr : AvailableQuests)
	{
		if (UQuestAsset* Quest = QuestPtr.Get())
		{
			LoadedQuests.Add(Quest);
			UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Loaded quest '%s'"), *Quest->QuestId.ToString());
		}
	}

	OnAvailableQuestsChanged.Broadcast();
}

TArray<UQuestAsset*> UQuestGiverComponent::GetAvailableQuestsForPlayer(APlayerController* Player) const
{
	TArray<UQuestAsset*> AvailableForPlayer;

	if (!Player)
	{
		return AvailableForPlayer;
	}

	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (!QuestSystem || !QuestSystem->GetProgressManager())
	{
		return AvailableForPlayer;
	}

	UQuestProgressManager* ProgressMgr = QuestSystem->GetProgressManager();

	for (UQuestAsset* Quest : LoadedQuests)
	{
		if (!Quest)
		{
			continue;
		}

		// Check if quest can be started
		TArray<FText> FailReasons;
		if (ProgressMgr->CanStartQuest(Quest->QuestId, FailReasons))
		{
			// Check if not already active or completed
			EQuestState State = ProgressMgr->GetQuestState(Quest->QuestId);
			if (State == EQuestState::NotStarted || 
				(State == EQuestState::Completed && Quest->Meta.bIsRepeatable))
			{
				AvailableForPlayer.Add(Quest);
			}
		}
	}

	return AvailableForPlayer;
}

TArray<UQuestAsset*> UQuestGiverComponent::GetActiveQuestsForPlayer(APlayerController* Player) const
{
	TArray<UQuestAsset*> ActiveQuests;

	if (!Player)
	{
		return ActiveQuests;
	}

	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (!QuestSystem || !QuestSystem->GetProgressManager())
	{
		return ActiveQuests;
	}

	UQuestProgressManager* ProgressMgr = QuestSystem->GetProgressManager();

	for (UQuestAsset* Quest : LoadedQuests)
	{
		if (!Quest)
		{
			continue;
		}

		if (ProgressMgr->IsQuestActive(Quest->QuestId))
		{
			ActiveQuests.Add(Quest);
		}
	}

	return ActiveQuests;
}

TArray<UQuestAsset*> UQuestGiverComponent::GetCompletableQuestsForPlayer(APlayerController* Player) const
{
	TArray<UQuestAsset*> CompletableQuests;

	if (!Player)
	{
		return CompletableQuests;
	}

	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (!QuestSystem || !QuestSystem->GetProgressManager())
	{
		return CompletableQuests;
	}

	UQuestProgressManager* ProgressMgr = QuestSystem->GetProgressManager();

	for (UQuestAsset* Quest : LoadedQuests)
	{
		if (!Quest)
		{
			continue;
		}

		if (ProgressMgr->IsQuestActive(Quest->QuestId))
		{
			// Check if all objectives are complete
			FQuestSaveData SaveData = ProgressMgr->GetQuestSaveData(Quest->QuestId);
			bool bAllObjectivesComplete = true;

			// Simple check - in production would validate all objectives
			// For now, just check if quest is active (turn-in quests)
			if (Quest->QuestGiverId == NpcId)
			{
				CompletableQuests.Add(Quest);
			}
		}
	}

	return CompletableQuests;
}

bool UQuestGiverComponent::AcceptQuest(APlayerController* Player, UQuestAsset* QuestAsset)
{
	if (!Player || !QuestAsset)
	{
		return false;
	}

	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (!QuestSystem)
	{
		return false;
	}

	// For now, use simple async start (callbacks handled by subsystem events)
	FOnQuestStartedDelegate EmptyDelegate;
	QuestSystem->StartQuestAsync(QuestAsset->QuestId, EmptyDelegate);

	// Broadcast immediately (actual start happens async)
	OnQuestAccepted.Broadcast(QuestAsset, Player);
	OnAvailableQuestsChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Player accepted quest '%s' from '%s'"), 
		*QuestAsset->QuestId.ToString(), *NpcId.ToString());

	return true;
}

bool UQuestGiverComponent::TurnInQuest(APlayerController* Player, UQuestAsset* QuestAsset)
{
	if (!Player || !QuestAsset)
	{
		return false;
	}

	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (!QuestSystem)
	{
		return false;
	}

	// Complete the quest
	bool bSuccess = QuestSystem->CompleteQuest(QuestAsset->QuestId);

	if (bSuccess)
	{
		OnQuestTurnedIn.Broadcast(QuestAsset, Player);
		OnAvailableQuestsChanged.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Player turned in quest '%s' to '%s'"), 
			*QuestAsset->QuestId.ToString(), *NpcId.ToString());
	}

	return bSuccess;
}

bool UQuestGiverComponent::HasQuestInteraction(APlayerController* Player) const
{
	if (!Player)
	{
		return false;
	}

	// Check if has available, active, or completable quests
	return GetAvailableQuestsForPlayer(Player).Num() > 0 ||
		   GetActiveQuestsForPlayer(Player).Num() > 0 ||
		   GetCompletableQuestsForPlayer(Player).Num() > 0;
}

UTexture2D* UQuestGiverComponent::GetQuestIndicatorIcon(APlayerController* Player) const
{
	if (!Player)
	{
		return nullptr;
	}

	// Priority: Complete > Available > In Progress
	if (GetCompletableQuestsForPlayer(Player).Num() > 0)
	{
		return QuestCompleteIcon;
	}

	if (GetAvailableQuestsForPlayer(Player).Num() > 0)
	{
		return QuestAvailableIcon;
	}

	if (GetActiveQuestsForPlayer(Player).Num() > 0)
	{
		return QuestInProgressIcon;
	}

	return nullptr;
}

void UQuestGiverComponent::OnPlayerInteract(APlayerController* Player)
{
	if (!Player)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("QuestGiverComponent: Player interacted with '%s'"), *NpcId.ToString());

	// Emit NPC talked event for quest conditions
	UQuestSubsystem* QuestSystem = GetQuestSubsystem();
	if (QuestSystem && QuestSystem->GetEventBus())
	{
		QuestSystem->GetEventBus()->EmitNpcTalked(
			NpcId,
			Player->GetPawn(),
			FGameplayTagContainer()
		);
	}

	// In production, this would open dialog UI showing available/completable quests
	// For now, just log
	TArray<UQuestAsset*> Available = GetAvailableQuestsForPlayer(Player);
	TArray<UQuestAsset*> Completable = GetCompletableQuestsForPlayer(Player);

	UE_LOG(LogTemp, Log, TEXT("  Available quests: %d"), Available.Num());
	UE_LOG(LogTemp, Log, TEXT("  Completable quests: %d"), Completable.Num());
}

UQuestSubsystem* UQuestGiverComponent::GetQuestSubsystem() const
{
	UWorld* World = GetWorld();
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

UQuestLogComponent* UQuestGiverComponent::GetQuestLogFromActor(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	// Try to get component directly from actor
	UQuestLogComponent* QuestLog = Actor->FindComponentByClass<UQuestLogComponent>();
	if (QuestLog)
	{
		return QuestLog;
	}

	// If actor is a Pawn, try to get from Controller
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		if (AController* Controller = Pawn->GetController())
		{
			QuestLog = Controller->FindComponentByClass<UQuestLogComponent>();
			if (QuestLog)
			{
				return QuestLog;
			}
		}
	}

	// If actor is a Controller, try to get from Pawn
	if (AController* Controller = Cast<AController>(Actor))
	{
		if (APawn* Pawn = Controller->GetPawn())
		{
			QuestLog = Pawn->FindComponentByClass<UQuestLogComponent>();
			if (QuestLog)
			{
				return QuestLog;
			}
		}
	}

	return nullptr;
}

APlayerController* UQuestGiverComponent::GetPlayerControllerFromActor(AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	// Direct cast
	if (APlayerController* PC = Cast<APlayerController>(Actor))
	{
		return PC;
	}

	// From Pawn
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Cast<APlayerController>(Pawn->GetController());
	}

	return nullptr;
}

TArray<UQuestAsset*> UQuestGiverComponent::GetAvailableQuestsForActor(AActor* Actor) const
{
	// Use existing method but with Actor instead of PlayerController
	APlayerController* PC = GetPlayerControllerFromActor(Actor);
	if (PC)
	{
		return GetAvailableQuestsForPlayer(PC);
	}

	// Fallback: return all available quests
	TArray<UQuestAsset*> Result;
	for (const TSoftObjectPtr<UQuestAsset>& QuestPtr : AvailableQuests)
	{
		if (UQuestAsset* Quest = QuestPtr.LoadSynchronous())
		{
			Result.Add(Quest);
		}
	}
	return Result;
}

bool UQuestGiverComponent::AcceptQuestForActor(UQuestAsset* QuestAsset, AActor* Actor)
{
	if (!QuestAsset || !Actor)
	{
		return false;
	}

	// Try to get PlayerController for existing method
	APlayerController* PC = GetPlayerControllerFromActor(Actor);
	if (PC)
	{
		return AcceptQuest(PC, QuestAsset);  // FIX: Correct order
	}

	UE_LOG(LogTemp, Warning, TEXT("QuestGiver: Cannot accept quest - no PlayerController found for actor %s"), 
		*Actor->GetName());
	return false;
}

void UQuestGiverComponent::OnActorInteract(AActor* InteractingActor)
{
	if (!InteractingActor)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("QuestGiver: Actor %s interacted with NPC %s"), 
		*InteractingActor->GetName(), *NpcId.ToString());

	// Get available quests
	TArray<UQuestAsset*> Quests = GetAvailableQuestsForActor(InteractingActor);
	
	if (Quests.Num() > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("QuestGiver: %d quests available"), Quests.Num());
	}
}
