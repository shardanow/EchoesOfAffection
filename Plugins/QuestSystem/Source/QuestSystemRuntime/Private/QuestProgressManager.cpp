// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestProgressManager.h"
#include "QuestEventBus.h"
#include "QuestAssetLoader.h"
#include "TimerManager.h"
#include "Engine/World.h"

UQuestProgressManager::UQuestProgressManager()
{
}

void UQuestProgressManager::Initialize(UQuestEventBus* InEventBus, UQuestAssetLoader* InAssetLoader)
{
	EventBus = InEventBus;
	AssetLoader = InAssetLoader;

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Initialized"));
}

void UQuestProgressManager::Shutdown()
{
	// Clear all timers
	if (UWorld* World = GetWorld())
	{
		for (auto& TimerPair : QuestTimers)
		{
			World->GetTimerManager().ClearTimer(TimerPair.Value);
		}
	}
	QuestTimers.Empty();

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Shutdown"));
}

// === Quest Lifecycle ===

void UQuestProgressManager::StartQuestAsync(FName QuestId, FOnQuestStartedCallback OnStarted)
{
	if (QuestId.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Cannot start quest with empty ID"));
		OnStarted.ExecuteIfBound(false, nullptr);
		return;
	}

	// Check if already active
	EQuestState CurrentState = GetQuestState(QuestId);
	if (CurrentState == EQuestState::Active)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Quest '%s' is already active"), *QuestId.ToString());
		OnStarted.ExecuteIfBound(false, nullptr);
		return;
	}

	// Load quest asset asynchronously
	AssetLoader->LoadQuestAssetAsyncNative(QuestId, FOnQuestAssetLoadedNative::CreateUObject(
		this, 
		&UQuestProgressManager::OnQuestAssetLoaded,
		QuestId,
		OnStarted
	));
}

void UQuestProgressManager::OnQuestAssetLoaded(UQuestAsset* QuestAsset, FName QuestId, FOnQuestStartedCallback OnStarted)
{
	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager::OnQuestAssetLoaded called for quest '%s', QuestAsset=%s"), 
		*QuestId.ToString(), QuestAsset ? TEXT("Valid") : TEXT("NULL"));
	
	if (!QuestAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestProgressManager: Cannot start quest '%s' - asset not found"), *QuestId.ToString());
		OnStarted.ExecuteIfBound(false, nullptr);
		return;
	}

	// Start quest
	bool bSuccess = StartQuestInternal(QuestId, QuestAsset);
	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: StartQuestInternal returned %s for quest '%s'"), 
		bSuccess ? TEXT("TRUE") : TEXT("FALSE"), *QuestId.ToString());
	
	OnStarted.ExecuteIfBound(bSuccess, bSuccess ? QuestAsset : nullptr);
	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: OnStarted callback executed for quest '%s'"), *QuestId.ToString());
}

bool UQuestProgressManager::StartQuestInternal(FName QuestId, UQuestAsset* QuestAsset)
{
	if (!QuestAsset)
	{
		return false;
	}

	// Check current state
	EQuestState CurrentState = GetQuestState(QuestId);
	
	// If already active, don't start again
	if (CurrentState == EQuestState::Active)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Quest '%s' is already active, skipping start"), *QuestId.ToString());
		return false;
	}

	// If completed and not repeatable, don't start
	if (CurrentState == EQuestState::Completed && !QuestAsset->Meta.bIsRepeatable)
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Quest '%s' is completed and not repeatable"), *QuestId.ToString());
		return false;
	}

	// Check dependencies
	TArray<FText> FailReasons;
	if (!CanStartQuest(QuestId, FailReasons))
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Cannot start quest '%s' - dependencies not met"), *QuestId.ToString());
		for (const FText& Reason : FailReasons)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - %s"), *Reason.ToString());
		}
		return false;
	}

	// Create or get save data
	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	EQuestState OldState = SaveData.State;
	SaveData.QuestId = QuestId;
	SaveData.State = EQuestState::Active;
	SaveData.StartTime = FDateTime::Now();
	SaveData.ElapsedTime = 0.0f;

	// Initialize first phase
	if (QuestAsset->Phases.Num() > 0)
	{
		const FQuestPhase& FirstPhase = QuestAsset->Phases[0];
		SaveData.CurrentPhaseId = FirstPhase.PhaseId;

		FPhaseSaveData& PhaseData = GetOrCreatePhaseSaveData(SaveData, FirstPhase.PhaseId);
		PhaseData.bIsActive = true;
		PhaseData.StartTime = FDateTime::Now();

		// Initialize objectives
		for (const FQuestObjective& Objective : FirstPhase.Objectives)
		{
			FObjectiveSaveData& ObjectiveData = GetOrCreateObjectiveSaveData(PhaseData, Objective.ObjectiveId);
			ObjectiveData.StartTime = FDateTime::Now();
			ObjectiveData.CurrentCount = 0;
			ObjectiveData.bIsCompleted = false;
		}
	}

	// Start timer if needed
	if (QuestAsset->FailurePolicy.FailureType == EQuestFailureType::TimeLimit)
	{
		StartQuestTimer(QuestId, QuestAsset);
	}

	// Broadcast events
	OnQuestStateChanged.Broadcast(QuestId, OldState, EQuestState::Active);
	OnQuestStarted.Broadcast(QuestId, QuestAsset);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Started quest '%s'"), *QuestId.ToString());

	return true;
}

bool UQuestProgressManager::CompleteQuest(FName QuestId)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	EQuestState OldState = SaveData.State;
	SaveData.State = EQuestState::Completed;
	SaveData.EndTime = FDateTime::Now();
	SaveData.CompletionCount++;

	// Clear timer
	if (UWorld* World = GetWorld())
	{
		if (FTimerHandle* Timer = QuestTimers.Find(QuestId))
		{
			World->GetTimerManager().ClearTimer(*Timer);
			QuestTimers.Remove(QuestId);
		}
	}

	// Broadcast events
	OnQuestStateChanged.Broadcast(QuestId, OldState, EQuestState::Completed);
	OnQuestCompleted.Broadcast(QuestId, QuestAsset);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Completed quest '%s'"), *QuestId.ToString());

	return true;
}

bool UQuestProgressManager::FailQuest(FName QuestId)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	EQuestState OldState = SaveData.State;
	SaveData.State = EQuestState::Failed;
	SaveData.EndTime = FDateTime::Now();

	// Reset progress if policy requires
	if (QuestAsset->FailurePolicy.bResetProgressOnFail)
	{
		for (FPhaseSaveData& PhaseData : SaveData.Phases)
		{
			for (FObjectiveSaveData& ObjectiveData : PhaseData.Objectives)
			{
				ObjectiveData.CurrentCount = 0;
				ObjectiveData.bIsCompleted = false;
			}
		}
	}

	// Clear timer
	if (UWorld* World = GetWorld())
	{
		if (FTimerHandle* Timer = QuestTimers.Find(QuestId))
		{
			World->GetTimerManager().ClearTimer(*Timer);
			QuestTimers.Remove(QuestId);
		}
	}

	// Broadcast events
	OnQuestStateChanged.Broadcast(QuestId, OldState, EQuestState::Failed);
	OnQuestFailed.Broadcast(QuestId, QuestAsset);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Failed quest '%s'"), *QuestId.ToString());

	return true;
}

bool UQuestProgressManager::AbandonQuest(FName QuestId)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	EQuestState OldState = SaveData.State;
	SaveData.State = EQuestState::Abandoned;
	SaveData.EndTime = FDateTime::Now();

	// Clear timer
	if (UWorld* World = GetWorld())
	{
		if (FTimerHandle* Timer = QuestTimers.Find(QuestId))
		{
			World->GetTimerManager().ClearTimer(*Timer);
			QuestTimers.Remove(QuestId);
		}
	}

	// Broadcast events
	OnQuestStateChanged.Broadcast(QuestId, OldState, EQuestState::Abandoned);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Abandoned quest '%s'"), *QuestId.ToString());

	return true;
}

bool UQuestProgressManager::RetryQuest(FName QuestId)
{
	EQuestState CurrentState = GetQuestState(QuestId);
	if (CurrentState != EQuestState::Failed)
	{
		return false;
	}

	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset || !QuestAsset->FailurePolicy.bAllowRetry)
	{
		return false;
	}

	// Reset and restart
	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	SaveData.Phases.Empty();
	SaveData.CurrentPhaseId = NAME_None;

	// Restart async
	StartQuestAsync(QuestId, FOnQuestStartedCallback());

	return true;
}

// === State Queries ===

EQuestState UQuestProgressManager::GetQuestState(FName QuestId) const
{
	if (!CurrentSaveGame)
	{
		return EQuestState::NotStarted;
	}

	FQuestSaveData SaveData = CurrentSaveGame->GetQuestData(QuestId);
	return SaveData.QuestId.IsNone() ? EQuestState::NotStarted : SaveData.State;
}

bool UQuestProgressManager::IsQuestActive(FName QuestId) const
{
	return GetQuestState(QuestId) == EQuestState::Active;
}

bool UQuestProgressManager::IsQuestCompleted(FName QuestId) const
{
	return GetQuestState(QuestId) == EQuestState::Completed;
}

TArray<FQuestSaveData> UQuestProgressManager::GetActiveQuests() const
{
	if (!CurrentSaveGame)
	{
		return TArray<FQuestSaveData>();
	}
	return CurrentSaveGame->GetQuestsByState(EQuestState::Active);
}

TArray<FQuestSaveData> UQuestProgressManager::GetCompletedQuests() const
{
	if (!CurrentSaveGame)
	{
		return TArray<FQuestSaveData>();
	}
	return CurrentSaveGame->GetQuestsByState(EQuestState::Completed);
}

FQuestSaveData UQuestProgressManager::GetQuestSaveData(FName QuestId) const
{
	if (!CurrentSaveGame)
	{
		return FQuestSaveData();
	}
	return CurrentSaveGame->GetQuestData(QuestId);
}

// === Objective Management ===

bool UQuestProgressManager::UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount)
{
	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ?? Called for Quest='%s', Objective='%s', Amount=%d"),
		*QuestId.ToString(), *ObjectiveId.ToString(), Amount);

	if (!IsQuestActive(QuestId))
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? Quest '%s' is NOT active!"), *QuestId.ToString());
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? QuestAsset NOT FOUND for '%s'!"), *QuestId.ToString());
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   Quest Asset found, CurrentPhaseId='%s'"),
		*SaveData.CurrentPhaseId.ToString());

	// Find current phase
	FPhaseSaveData* CurrentPhase = nullptr;
	for (FPhaseSaveData& Phase : SaveData.Phases)
	{
		if (Phase.PhaseId == SaveData.CurrentPhaseId && Phase.bIsActive)
		{
			CurrentPhase = &Phase;
			break;
		}
	}

	if (!CurrentPhase)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? CurrentPhase NOT FOUND for PhaseId='%s'!"),
			*SaveData.CurrentPhaseId.ToString());
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   CurrentPhase found with %d objectives"),
		CurrentPhase->Objectives.Num());

	// Find objective
	FObjectiveSaveData* ObjectiveData = nullptr;
	for (FObjectiveSaveData& Obj : CurrentPhase->Objectives)
	{
		if (Obj.ObjectiveId == ObjectiveId)
		{
			ObjectiveData = &Obj;
			break;
		}
	}

	if (!ObjectiveData)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? ObjectiveData NOT FOUND for ObjectiveId='%s'!"),
			*ObjectiveId.ToString());
		return false;
	}

	if (ObjectiveData->bIsCompleted)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? Objective '%s' is ALREADY COMPLETED!"),
			*ObjectiveId.ToString());
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   ObjectiveData found, CurrentCount=%d, bIsCompleted=%d"),
		ObjectiveData->CurrentCount, ObjectiveData->bIsCompleted);

	// Get objective definition
	FQuestPhase PhaseDefinition = QuestAsset->GetPhase(SaveData.CurrentPhaseId);
	const FQuestObjective* ObjectiveDef = nullptr;
	for (const FQuestObjective& Obj : PhaseDefinition.Objectives)
	{
		if (Obj.ObjectiveId == ObjectiveId)
		{
			ObjectiveDef = &Obj;
			break;
		}
	}

	if (!ObjectiveDef)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? ObjectiveDef NOT FOUND in QuestAsset for ObjectiveId='%s'!"),
			*ObjectiveId.ToString());
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   ObjectiveDef found, TargetCount=%d"),
		ObjectiveDef->Counters.TargetCount);

	// Check gates
	if (!CheckProgressGates(*ObjectiveDef))
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Objective '%s' gates not met"), *ObjectiveId.ToString());
		return false;
	}

	// Update progress
	ObjectiveData->CurrentCount += Amount;

	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ? Progress updated! NewCount=%d (Added %d)"),
		ObjectiveData->CurrentCount, Amount);

	// Clamp if needed
	if (!ObjectiveDef->Counters.bAllowOvercount)
	{
		int32 OldCount = ObjectiveData->CurrentCount;
		ObjectiveData->CurrentCount = FMath::Min(ObjectiveData->CurrentCount, ObjectiveDef->Counters.TargetCount);
		if (OldCount != ObjectiveData->CurrentCount)
		{
			UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   Clamped from %d to %d (TargetCount=%d)"),
				OldCount, ObjectiveData->CurrentCount, ObjectiveDef->Counters.TargetCount);
		}
	}

	// Broadcast update
	UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ?? Broadcasting OnObjectiveUpdated for Quest='%s', Objective='%s'"),
		*QuestId.ToString(), *ObjectiveId.ToString());
	OnObjectiveUpdated.Broadcast(QuestId, ObjectiveId);

	// Check completion
	if (ObjectiveData->CurrentCount >= ObjectiveDef->Counters.TargetCount)
	{
		ObjectiveData->bIsCompleted = true;
		ObjectiveData->CompletionTime = FDateTime::Now();

		UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress] ??? OBJECTIVE COMPLETED! Quest='%s', Objective='%s'"),
			*QuestId.ToString(), *ObjectiveId.ToString());
		OnObjectiveCompleted.Broadcast(QuestId, ObjectiveId);

		UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Completed objective '%s' in quest '%s'"), 
			*ObjectiveId.ToString(), *QuestId.ToString());

		// Check if phase is complete
		bool bAllObjectivesComplete = true;
		if (PhaseDefinition.bRequireAllObjectives)
		{
			for (const FObjectiveSaveData& Obj : CurrentPhase->Objectives)
			{
				const FQuestObjective* ObjDef = nullptr;
				for (const FQuestObjective& DefObj : PhaseDefinition.Objectives)
				{
					if (DefObj.ObjectiveId == Obj.ObjectiveId)
					{
						ObjDef = &DefObj;
						break;
					}
				}

				if (ObjDef && !ObjDef->bIsOptional && !Obj.bIsCompleted)
				{
					bAllObjectivesComplete = false;
					break;
				}
			}
		}

		if (bAllObjectivesComplete)
		{
			CurrentPhase->bIsCompleted = true;
			CurrentPhase->CompletionTime = FDateTime::Now();

			// Auto-advance if configured
			if (PhaseDefinition.Transition.bAutoAdvance)
			{
				if (!PhaseDefinition.Transition.NextPhaseId.IsNone())
				{
					// Has next phase - advance and process phase rewards in OnPhaseChanged
					AdvanceToNextPhase(QuestId);
				}
				else
				{
					// No next phase - this is the last phase
					// Trigger phase change event to process phase rewards BEFORE completing quest
					UE_LOG(LogTemp, Warning, TEXT("?? Last phase '%s' completed - broadcasting phase change for rewards"), *SaveData.CurrentPhaseId.ToString());
					OnQuestPhaseChanged.Broadcast(QuestId, SaveData.CurrentPhaseId, NAME_None);
					
					// Now complete the quest (quest completion rewards processed in OnQuestCompleted)
					CompleteQuest(QuestId);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[QuestProgressManager::UpdateObjectiveProgress]   Objective NOT YET completed: %d/%d"),
			ObjectiveData->CurrentCount, ObjectiveDef->Counters.TargetCount);
	}

	return true;
}

bool UQuestProgressManager::SetObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Progress)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	FPhaseSaveData* CurrentPhase = nullptr;
	
	for (FPhaseSaveData& Phase : SaveData.Phases)
	{
		if (Phase.PhaseId == SaveData.CurrentPhaseId)
		{
			CurrentPhase = &Phase;
			break;
		}
	}

	if (!CurrentPhase)
	{
		return false;
	}

	for (FObjectiveSaveData& Obj : CurrentPhase->Objectives)
	{
		if (Obj.ObjectiveId == ObjectiveId)
		{
			int32 Delta = Progress - Obj.CurrentCount;
			return UpdateObjectiveProgress(QuestId, ObjectiveId, Delta);
		}
	}

	return false;
}

bool UQuestProgressManager::CompleteObjective(FName QuestId, FName ObjectiveId)
{
	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return false;
	}

	FQuestPhase PhaseDefinition = QuestAsset->GetPhase(GetCurrentPhaseId(QuestId));
	for (const FQuestObjective& Obj : PhaseDefinition.Objectives)
	{
		if (Obj.ObjectiveId == ObjectiveId)
		{
			return SetObjectiveProgress(QuestId, ObjectiveId, Obj.Counters.TargetCount);
		}
	}

	return false;
}

void UQuestProgressManager::GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const
{
	CurrentCount = 0;
	TargetCount = 1;
	bIsCompleted = false;

	FQuestSaveData SaveData = GetQuestSaveData(QuestId);
	if (SaveData.QuestId.IsNone())
	{
		return;
	}

	for (const FPhaseSaveData& Phase : SaveData.Phases)
	{
		if (Phase.PhaseId == SaveData.CurrentPhaseId)
		{
			for (const FObjectiveSaveData& Obj : Phase.Objectives)
			{
				if (Obj.ObjectiveId == ObjectiveId)
				{
					CurrentCount = Obj.CurrentCount;
					bIsCompleted = Obj.bIsCompleted;

					// Get target from asset
					UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
					if (QuestAsset)
					{
						FQuestPhase PhaseDef = QuestAsset->GetPhase(Phase.PhaseId);
						for (const FQuestObjective& ObjDef : PhaseDef.Objectives)
						{
							if (ObjDef.ObjectiveId == ObjectiveId)
							{
								TargetCount = ObjDef.Counters.TargetCount;
								return;
							}
						}
					}
					return;
				}
			}
		}
	}
}

// === Phase Management ===

bool UQuestProgressManager::AdvanceToNextPhase(FName QuestId)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	FQuestPhase CurrentPhaseDef = QuestAsset->GetPhase(SaveData.CurrentPhaseId);

	if (CurrentPhaseDef.Transition.NextPhaseId.IsNone())
	{
		return CompleteQuest(QuestId);
	}

	return SetQuestPhase(QuestId, CurrentPhaseDef.Transition.NextPhaseId);
}

bool UQuestProgressManager::SetQuestPhase(FName QuestId, FName PhaseId)
{
	if (!IsQuestActive(QuestId))
	{
		return false;
	}

	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset || !QuestAsset->HasPhase(PhaseId))
	{
		return false;
	}

	FQuestSaveData& SaveData = GetOrCreateQuestSaveData(QuestId);
	FName OldPhaseId = SaveData.CurrentPhaseId;

	// Deactivate old phase
	for (FPhaseSaveData& Phase : SaveData.Phases)
	{
		if (Phase.PhaseId == OldPhaseId)
		{
			Phase.bIsActive = false;
		}
	}

	// Activate new phase
	SaveData.CurrentPhaseId = PhaseId;
	FPhaseSaveData& NewPhaseData = GetOrCreatePhaseSaveData(SaveData, PhaseId);
	NewPhaseData.bIsActive = true;
	NewPhaseData.StartTime = FDateTime::Now();

	// Initialize objectives
	FQuestPhase PhaseDef = QuestAsset->GetPhase(PhaseId);
	for (const FQuestObjective& Objective : PhaseDef.Objectives)
	{
		FObjectiveSaveData& ObjectiveData = GetOrCreateObjectiveSaveData(NewPhaseData, Objective.ObjectiveId);
		if (ObjectiveData.CurrentCount == 0)
		{
			ObjectiveData.StartTime = FDateTime::Now();
			ObjectiveData.bIsCompleted = false;
		}
	}

	// Broadcast
	OnQuestPhaseChanged.Broadcast(QuestId, OldPhaseId, PhaseId);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Quest '%s' advanced to phase '%s'"), 
		*QuestId.ToString(), *PhaseId.ToString());

	// ? NEW: Broadcast phase completed for reward processing
	// QuestSubsystem will listen to OnQuestPhaseChanged and process phase rewards
	if (!OldPhaseId.IsNone())
	{
		// Phase rewards are processed in QuestSubsystem::OnPhaseChanged handler
		UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Phase '%s' completed, broadcasting for rewards"), *OldPhaseId.ToString());
	}

	return true;
}

FName UQuestProgressManager::GetCurrentPhaseId(FName QuestId) const
{
	FQuestSaveData SaveData = GetQuestSaveData(QuestId);
	return SaveData.CurrentPhaseId;
}

// === Dependencies ===

bool UQuestProgressManager::CanStartQuest(FName QuestId, TArray<FText>& OutFailReasons) const
{
	UQuestAsset* QuestAsset = AssetLoader->GetLoadedQuestAsset(QuestId);
	if (!QuestAsset)
	{
		OutFailReasons.Add(FText::FromString(TEXT("Quest asset not found")));
		return false;
	}

	return CheckDependencies(QuestAsset, OutFailReasons);
}

// === Internal Methods ===

bool UQuestProgressManager::CheckDependencies(const UQuestAsset* QuestAsset, TArray<FText>& OutFailReasons) const
{
	if (!QuestAsset)
	{
		return false;
	}

	bool bAllMet = true;

	for (const FQuestDependency& Dependency : QuestAsset->Dependencies)
	{
		if (!Dependency.RequiredQuestId.IsNone())
		{
			EQuestState RequiredState = GetQuestState(Dependency.RequiredQuestId);
			if (RequiredState != Dependency.RequiredQuestState)
			{
				OutFailReasons.Add(FText::Format(
					FText::FromString(TEXT("Required quest '{0}' not in state '{1}'")),
					FText::FromName(Dependency.RequiredQuestId),
					FText::FromString(StaticEnum<EQuestState>()->GetValueAsString(Dependency.RequiredQuestState))
				));
				bAllMet = false;
			}
		}
	}

	return bAllMet;
}

bool UQuestProgressManager::CheckProgressGates(const FQuestObjective& Objective) const
{
	// Placeholder - integrate with game systems
	return true;
}

bool UQuestProgressManager::EvaluateCondition(const FObjectiveCondition& Condition, const FQuestEventPayload& Payload) const
{
	// Placeholder - will be used by event system
	return true;
}

void UQuestProgressManager::StartQuestTimer(FName QuestId, const UQuestAsset* QuestAsset)
{
	if (!QuestAsset || QuestAsset->FailurePolicy.FailureType != EQuestFailureType::TimeLimit)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	float TimeLimit = QuestAsset->FailurePolicy.TimeLimit;
	if (TimeLimit <= 0.0f)
	{
		return;
	}

	FTimerHandle& TimerHandle = QuestTimers.FindOrAdd(QuestId);
	World->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateUObject(this, &UQuestProgressManager::OnQuestTimerExpired, QuestId),
		TimeLimit,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("QuestProgressManager: Started timer for quest '%s' (%.1f seconds)"), 
		*QuestId.ToString(), TimeLimit);
}

void UQuestProgressManager::OnQuestTimerExpired(FName QuestId)
{
	UE_LOG(LogTemp, Warning, TEXT("QuestProgressManager: Quest '%s' timer expired"), *QuestId.ToString());
	FailQuest(QuestId);
}

FQuestSaveData& UQuestProgressManager::GetOrCreateQuestSaveData(FName QuestId)
{
	if (!CurrentSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("QuestProgressManager: No save game set!"));
		static FQuestSaveData EmptyData;
		return EmptyData;
	}

	for (FQuestSaveData& Data : CurrentSaveGame->QuestProgress)
	{
		if (Data.QuestId == QuestId)
		{
			return Data;
		}
	}

	// Create new
	FQuestSaveData NewData;
	NewData.QuestId = QuestId;
	CurrentSaveGame->QuestProgress.Add(NewData);
	return CurrentSaveGame->QuestProgress.Last();
}

FPhaseSaveData& UQuestProgressManager::GetOrCreatePhaseSaveData(FQuestSaveData& QuestData, FName PhaseId)
{
	for (FPhaseSaveData& Data : QuestData.Phases)
	{
		if (Data.PhaseId == PhaseId)
		{
			return Data;
		}
	}

	FPhaseSaveData NewData;
	NewData.PhaseId = PhaseId;
	QuestData.Phases.Add(NewData);
	return QuestData.Phases.Last();
}

FObjectiveSaveData& UQuestProgressManager::GetOrCreateObjectiveSaveData(FPhaseSaveData& PhaseData, FName ObjectiveId)
{
	for (FObjectiveSaveData& Data : PhaseData.Objectives)
	{
		if (Data.ObjectiveId == ObjectiveId)
		{
			return Data;
		}
	}

	FObjectiveSaveData NewData;
	NewData.ObjectiveId = ObjectiveId;
	PhaseData.Objectives.Add(NewData);
	return PhaseData.Objectives.Last();
}

UQuestAsset* UQuestProgressManager::GetQuestAsset(FName QuestId) const
{
	return AssetLoader ? AssetLoader->GetLoadedQuestAsset(QuestId) : nullptr;
}
