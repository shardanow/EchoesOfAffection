// Copyright Epic Games, Inc. All Rights Reserved.

#include "Debug/QuestCheatManager.h"
#include "QuestSubsystem.h"
#include "QuestSaveManager.h"
#include "QuestEventBus.h"
#include "QuestAsset.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UQuestSubsystem* UQuestCheatManager::GetQuestSubsystem() const
{
	return UQuestSubsystem::Get(GetWorld());
}

void UQuestCheatManager::PrintScreen(const FString& Message, FColor Color, float Duration)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
	}
	UE_LOG(LogTemp, Log, TEXT("QuestCheat: %s"), *Message);
}

// === Quest Control ===

void UQuestCheatManager::StartQuest(FName QuestId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	// Use empty callback - we'll check success in log
	FOnQuestStartedDelegate Callback;
	QuestSys->StartQuestAsync(QuestId, Callback);

	PrintScreen(FString::Printf(TEXT("Starting quest '%s'..."), *QuestId.ToString()), FColor::Cyan);
}

void UQuestCheatManager::CompleteQuest(FName QuestId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->CompleteQuest(QuestId);
	if (bSuccess)
	{
		PrintScreen(FString::Printf(TEXT("Quest '%s' completed!"), *QuestId.ToString()), FColor::Green);
	}
	else
	{
		PrintScreen(FString::Printf(TEXT("Failed to complete quest '%s'"), *QuestId.ToString()), FColor::Red);
	}
}

void UQuestCheatManager::FailQuest(FName QuestId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->FailQuest(QuestId);
	if (bSuccess)
	{
		PrintScreen(FString::Printf(TEXT("Quest '%s' failed!"), *QuestId.ToString()), FColor::Orange);
	}
	else
	{
		PrintScreen(FString::Printf(TEXT("Failed to fail quest '%s'"), *QuestId.ToString()), FColor::Red);
	}
}

void UQuestCheatManager::AbandonQuest(FName QuestId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->AbandonQuest(QuestId);
	if (bSuccess)
	{
		PrintScreen(FString::Printf(TEXT("Quest '%s' abandoned!"), *QuestId.ToString()), FColor::Orange);
	}
	else
	{
		PrintScreen(FString::Printf(TEXT("Failed to abandon quest '%s'"), *QuestId.ToString()), FColor::Red);
	}
}

void UQuestCheatManager::CompleteObjective(FName QuestId, FName ObjectiveId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->CompleteObjective(QuestId, ObjectiveId);
	if (bSuccess)
	{
		PrintScreen(FString::Printf(TEXT("Objective '%s.%s' completed!"), 
			*QuestId.ToString(), *ObjectiveId.ToString()), FColor::Green);
	}
	else
	{
		PrintScreen(FString::Printf(TEXT("Failed to complete objective '%s.%s'"), 
			*QuestId.ToString(), *ObjectiveId.ToString()), FColor::Red);
	}
}

void UQuestCheatManager::UpdateObjective(FName QuestId, FName ObjectiveId, int32 Amount)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->UpdateObjectiveProgress(QuestId, ObjectiveId, Amount);
	if (bSuccess)
	{
		int32 Current, Target;
		bool bCompleted;
		QuestSys->GetObjectiveProgress(QuestId, ObjectiveId, Current, Target, bCompleted);

		PrintScreen(FString::Printf(TEXT("Objective '%s.%s': %d/%d"), 
			*QuestId.ToString(), *ObjectiveId.ToString(), Current, Target), FColor::Cyan);
	}
	else
	{
		PrintScreen(FString::Printf(TEXT("Failed to update objective '%s.%s'"), 
			*QuestId.ToString(), *ObjectiveId.ToString()), FColor::Red);
	}
}

// === Quest Info ===

void UQuestCheatManager::ShowActiveQuests()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	TArray<FQuestSaveData> ActiveQuests = QuestSys->GetActiveQuests();

	PrintScreen(FString::Printf(TEXT("=== ACTIVE QUESTS (%d) ==="), ActiveQuests.Num()), FColor::Yellow);

	for (const FQuestSaveData& Quest : ActiveQuests)
	{
		UQuestAsset* QuestAsset = QuestSys->GetQuestAsset(Quest.QuestId);
		FString QuestName = QuestAsset ? QuestAsset->Title.ToString() : Quest.QuestId.ToString();

		PrintScreen(FString::Printf(TEXT("  - %s (Phase: %s)"), 
			*QuestName, *Quest.CurrentPhaseId.ToString()), FColor::White);
	}

	if (ActiveQuests.Num() == 0)
	{
		PrintScreen(TEXT("  (No active quests)"), FColor::Silver);
	}
}

void UQuestCheatManager::ShowCompletedQuests()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	TArray<FQuestSaveData> CompletedQuests = QuestSys->GetCompletedQuests();

	PrintScreen(FString::Printf(TEXT("=== COMPLETED QUESTS (%d) ==="), CompletedQuests.Num()), FColor::Green);

	for (const FQuestSaveData& Quest : CompletedQuests)
	{
		UQuestAsset* QuestAsset = QuestSys->GetQuestAsset(Quest.QuestId);
		FString QuestName = QuestAsset ? QuestAsset->Title.ToString() : Quest.QuestId.ToString();

		PrintScreen(FString::Printf(TEXT("  - %s"), *QuestName), FColor::White);
	}

	if (CompletedQuests.Num() == 0)
	{
		PrintScreen(TEXT("  (No completed quests)"), FColor::Silver);
	}
}

void UQuestCheatManager::ShowQuestInfo(FName QuestId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	FQuestSaveData SaveData = QuestSys->GetQuestSaveData(QuestId);
	if (SaveData.QuestId.IsNone())
	{
		PrintScreen(FString::Printf(TEXT("Quest '%s' not found!"), *QuestId.ToString()), FColor::Red);
		return;
	}

	UQuestAsset* QuestAsset = QuestSys->GetQuestAsset(QuestId);
	FString QuestName = QuestAsset ? QuestAsset->Title.ToString() : QuestId.ToString();

	PrintScreen(FString::Printf(TEXT("=== QUEST INFO: %s ==="), *QuestName), FColor::Cyan);
	PrintScreen(FString::Printf(TEXT("  ID: %s"), *QuestId.ToString()), FColor::White);
	PrintScreen(FString::Printf(TEXT("  State: %s"), *UEnum::GetValueAsString(SaveData.State)), FColor::White);
	PrintScreen(FString::Printf(TEXT("  Current Phase: %s"), *SaveData.CurrentPhaseId.ToString()), FColor::White);

	// Show objectives
	for (const FPhaseSaveData& Phase : SaveData.Phases)
	{
		if (Phase.bIsActive)
		{
			PrintScreen(FString::Printf(TEXT("  Objectives (%d):"), Phase.Objectives.Num()), FColor::White);
			for (const FObjectiveSaveData& Objective : Phase.Objectives)
			{
				// Get target from asset
				int32 TargetCount = 1;
				if (QuestAsset)
				{
					FQuestPhase PhaseDef = QuestAsset->GetPhase(Phase.PhaseId);
					for (const FQuestObjective& ObjDef : PhaseDef.Objectives)
					{
						if (ObjDef.ObjectiveId == Objective.ObjectiveId)
						{
							TargetCount = ObjDef.Counters.TargetCount;
							break;
						}
					}
				}

				FString Status = Objective.bIsCompleted ? TEXT("[DONE]") : TEXT("[    ]");
				PrintScreen(FString::Printf(TEXT("    %s %s: %d/%d"), 
					*Status, *Objective.ObjectiveId.ToString(), 
					Objective.CurrentCount, TargetCount), 
					Objective.bIsCompleted ? FColor::Green : FColor::Yellow);
			}
		}
	}
}

// === Save/Load ===

void UQuestCheatManager::ResetQuests()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys || !QuestSys->GetSaveManager())
	{
		PrintScreen(TEXT("Quest System or Save Manager not found!"), FColor::Red);
		return;
	}

	bool bSuccess = QuestSys->GetSaveManager()->DeleteSaveSlot("QuestSaveSlot", 0);

	if (bSuccess)
	{
		PrintScreen(TEXT("Quest saves DELETED! Restart level to apply."), FColor::Orange);
		
		// Optional: Restart level automatically
		// UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
	}
	else
	{
		PrintScreen(TEXT("Failed to delete quest saves!"), FColor::Red);
	}
}

void UQuestCheatManager::ShowQuestSaves()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	TArray<FQuestSaveData> ActiveQuests = QuestSys->GetActiveQuests();
	TArray<FQuestSaveData> CompletedQuests = QuestSys->GetCompletedQuests();

	PrintScreen(TEXT("=== QUEST SAVE FILE ==="), FColor::Cyan);
	PrintScreen(FString::Printf(TEXT("  Active Quests: %d"), ActiveQuests.Num()), FColor::Yellow);
	PrintScreen(FString::Printf(TEXT("  Completed Quests: %d"), CompletedQuests.Num()), FColor::Green);

	bool bSaveExists = QuestSys->GetSaveManager()->DoesSaveSlotExist("QuestSaveSlot", 0);
	PrintScreen(FString::Printf(TEXT("  Save File Exists: %s"), bSaveExists ? TEXT("YES") : TEXT("NO")), 
		bSaveExists ? FColor::Green : FColor::Red);
}

void UQuestCheatManager::SaveQuests()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	QuestSys->SaveAsync();
	PrintScreen(TEXT("Saving quest progress..."), FColor::Cyan);
}

void UQuestCheatManager::LoadQuests()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	QuestSys->LoadAsync();
	PrintScreen(TEXT("Loading quest progress..."), FColor::Cyan);
}

void UQuestCheatManager::EnableAutoSave(float IntervalSeconds)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys)
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	QuestSys->EnableAutoSave(IntervalSeconds);
	PrintScreen(FString::Printf(TEXT("Auto-save ENABLED (interval: %.1f seconds)"), IntervalSeconds), FColor::Green);
}

void UQuestCheatManager::DisableAutoSave()
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys || !QuestSys->GetSaveManager())
	{
		PrintScreen(TEXT("Quest System not found!"), FColor::Red);
		return;
	}

	QuestSys->GetSaveManager()->DisableAutoSave();
	PrintScreen(TEXT("Auto-save DISABLED"), FColor::Orange);
}

// === Events ===

void UQuestCheatManager::EmitNpcTalked(FName NpcId)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys || !QuestSys->GetEventBus())
	{
		PrintScreen(TEXT("Quest System or Event Bus not found!"), FColor::Red);
		return;
	}

	APawn* PlayerPawn = GetOuterAPlayerController()->GetPawn();
	QuestSys->GetEventBus()->EmitNpcTalked(NpcId, PlayerPawn, FGameplayTagContainer());

	PrintScreen(FString::Printf(TEXT("Emitted NpcTalked event: %s"), *NpcId.ToString()), FColor::Cyan);
}

void UQuestCheatManager::EmitItemAcquired(FName ItemId, int32 Count)
{
	UQuestSubsystem* QuestSys = GetQuestSubsystem();
	if (!QuestSys || !QuestSys->GetEventBus())
	{
		PrintScreen(TEXT("Quest System or Event Bus not found!"), FColor::Red);
		return;
	}

	APawn* PlayerPawn = GetOuterAPlayerController()->GetPawn();
	QuestSys->GetEventBus()->EmitItemAcquired(ItemId, Count, PlayerPawn, FGameplayTagContainer());

	PrintScreen(FString::Printf(TEXT("Emitted ItemAcquired event: %s x%d"), *ItemId.ToString(), Count), FColor::Cyan);
}
