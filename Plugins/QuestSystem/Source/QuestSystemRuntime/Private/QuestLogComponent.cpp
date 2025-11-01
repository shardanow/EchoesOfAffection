// Copyright Epic Games, Inc. All Rights Reserved.

#include "QuestLogComponent.h"
#include "QuestSubsystem.h"
#include "Engine/World.h"

UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // No tick needed - event driven!
}

void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();

	QuestSubsystem = GetQuestSubsystem();
	SubscribeToQuestEvents();
}

void UQuestLogComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnsubscribeFromQuestEvents();
	Super::EndPlay(EndPlayReason);
}

UQuestSubsystem* UQuestLogComponent::GetQuestSubsystem() const
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

// Quest Operations

bool UQuestLogComponent::StartQuest(FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return false;
	}

	// Use async version with empty callback
	FOnQuestStartedDelegate EmptyCallback;
	Subsystem->StartQuestAsync(QuestId, EmptyCallback);
	return true;  // Returns immediately
}

bool UQuestLogComponent::AbandonQuest(FName QuestId)
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return false;
	}

	// Clear tracked if abandoning tracked quest
	if (TrackedQuestId == QuestId)
	{
		ClearTrackedQuest();
	}

	return Subsystem->AbandonQuest(QuestId);
}

bool UQuestLogComponent::IsQuestActive(FName QuestId) const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->IsQuestActive(QuestId);
}

bool UQuestLogComponent::IsQuestCompleted(FName QuestId) const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return false;
	}

	return Subsystem->IsQuestCompleted(QuestId);
}

EQuestState UQuestLogComponent::GetQuestState(FName QuestId) const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return EQuestState::NotStarted;
	}

	return Subsystem->GetQuestState(QuestId);
}

TArray<FQuestSaveData> UQuestLogComponent::GetActiveQuests() const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return TArray<FQuestSaveData>();
	}

	return Subsystem->GetActiveQuests();
}

TArray<FQuestSaveData> UQuestLogComponent::GetCompletedQuests() const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return TArray<FQuestSaveData>();
	}

	return Subsystem->GetCompletedQuests();
}

UQuestAsset* UQuestLogComponent::GetQuestAsset(FName QuestId) const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetQuestAsset(QuestId);
}

void UQuestLogComponent::GetObjectiveProgress(FName QuestId, FName ObjectiveId, int32& CurrentCount, int32& TargetCount, bool& bIsCompleted) const
{
	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		CurrentCount = 0;
		TargetCount = 1;
		bIsCompleted = false;
		return;
	}

	Subsystem->GetObjectiveProgress(QuestId, ObjectiveId, CurrentCount, TargetCount, bIsCompleted);
}

// Tracked Quest

void UQuestLogComponent::SetTrackedQuest(FName QuestId)
{
	if (!IsQuestActive(QuestId))
	{
		UE_LOG(LogTemp, Warning, TEXT("QuestLogComponent: Cannot track inactive quest '%s'"), *QuestId.ToString());
		return;
	}

	TrackedQuestId = QuestId;
	UE_LOG(LogTemp, Log, TEXT("QuestLogComponent: Now tracking quest '%s'"), *QuestId.ToString());
}

void UQuestLogComponent::ClearTrackedQuest()
{
	TrackedQuestId = NAME_None;
}

// UI Helpers

FText UQuestLogComponent::GetCurrentObjectiveText(FName QuestId) const
{
	UQuestAsset* QuestAsset = GetQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return FText::GetEmpty();
	}

	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return FText::GetEmpty();
	}

	FName CurrentPhaseId = Subsystem->GetCurrentPhaseId(QuestId);
	FQuestPhase Phase = QuestAsset->GetPhase(CurrentPhaseId);

	// Find first incomplete objective
	for (const FQuestObjective& Objective : Phase.Objectives)
	{
		int32 CurrentCount, TargetCount;
		bool bIsCompleted;
		GetObjectiveProgress(QuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

		if (!bIsCompleted && !Objective.bIsHidden)
		{
			// Format with progress
			if (TargetCount > 1)
			{
				return FText::Format(
					FText::FromString(TEXT("{0} ({1}/{2})")),
					Objective.Title,
					FText::AsNumber(CurrentCount),
					FText::AsNumber(TargetCount)
				);
			}
			else
			{
				return Objective.Title;
			}
		}
	}

	return FText::GetEmpty();
}

TArray<FText> UQuestLogComponent::GetActiveObjectiveTexts(FName QuestId) const
{
	TArray<FText> Texts;

	UQuestAsset* QuestAsset = GetQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return Texts;
	}

	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return Texts;
	}

	FName CurrentPhaseId = Subsystem->GetCurrentPhaseId(QuestId);
	FQuestPhase Phase = QuestAsset->GetPhase(CurrentPhaseId);

	// Get all incomplete objectives
	for (const FQuestObjective& Objective : Phase.Objectives)
	{
		int32 CurrentCount, TargetCount;
		bool bIsCompleted;
		GetObjectiveProgress(QuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

		if (!bIsCompleted && !Objective.bIsHidden)
		{
			if (TargetCount > 1)
			{
				Texts.Add(FText::Format(
					FText::FromString(TEXT("{0} ({1}/{2})")),
					Objective.Title,
					FText::AsNumber(CurrentCount),
					FText::AsNumber(TargetCount)
				));
			}
			else
			{
				Texts.Add(Objective.Title);
			}
		}
	}

	return Texts;
}

float UQuestLogComponent::GetQuestCompletionPercent(FName QuestId) const
{
	UQuestAsset* QuestAsset = GetQuestAsset(QuestId);
	if (!QuestAsset)
	{
		return 0.0f;
	}

	UQuestSubsystem* Subsystem = GetQuestSubsystem();
	if (!Subsystem)
	{
		return 0.0f;
	}

	int32 TotalObjectives = 0;
	int32 CompletedObjectives = 0;

	// Count all objectives across all phases
	for (const FQuestPhase& Phase : QuestAsset->Phases)
	{
		for (const FQuestObjective& Objective : Phase.Objectives)
		{
			if (!Objective.bIsOptional)
			{
				TotalObjectives++;

				int32 CurrentCount, TargetCount;
				bool bIsCompleted;
				GetObjectiveProgress(QuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

				if (bIsCompleted)
				{
					CompletedObjectives++;
				}
			}
		}
	}

	if (TotalObjectives == 0)
	{
		return 0.0f;
	}

	return static_cast<float>(CompletedObjectives) / static_cast<float>(TotalObjectives);
}

// Event Subscription

void UQuestLogComponent::SubscribeToQuestEvents()
{
	if (bIsSubscribed || !QuestSubsystem || !QuestSubsystem->GetProgressManager())
	{
		return;
	}

	auto* ProgressMgr = QuestSubsystem->GetProgressManager();
	ProgressMgr->OnQuestStarted.AddDynamic(this, &UQuestLogComponent::OnQuestStartedHandler);
	ProgressMgr->OnQuestCompleted.AddDynamic(this, &UQuestLogComponent::OnQuestCompletedHandler);
	ProgressMgr->OnQuestFailed.AddDynamic(this, &UQuestLogComponent::OnQuestFailedHandler);
	ProgressMgr->OnObjectiveUpdated.AddDynamic(this, &UQuestLogComponent::OnObjectiveUpdatedHandler);
	ProgressMgr->OnObjectiveCompleted.AddDynamic(this, &UQuestLogComponent::OnObjectiveCompletedHandler);

	bIsSubscribed = true;
}

void UQuestLogComponent::UnsubscribeFromQuestEvents()
{
	if (!bIsSubscribed || !QuestSubsystem || !QuestSubsystem->GetProgressManager())
	{
		return;
	}

	auto* ProgressMgr = QuestSubsystem->GetProgressManager();
	ProgressMgr->OnQuestStarted.RemoveDynamic(this, &UQuestLogComponent::OnQuestStartedHandler);
	ProgressMgr->OnQuestCompleted.RemoveDynamic(this, &UQuestLogComponent::OnQuestCompletedHandler);
	ProgressMgr->OnQuestFailed.RemoveDynamic(this, &UQuestLogComponent::OnQuestFailedHandler);
	ProgressMgr->OnObjectiveUpdated.RemoveDynamic(this, &UQuestLogComponent::OnObjectiveUpdatedHandler);
	ProgressMgr->OnObjectiveCompleted.RemoveDynamic(this, &UQuestLogComponent::OnObjectiveCompletedHandler);

	bIsSubscribed = false;
}

// Event Handlers

void UQuestLogComponent::OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Log, TEXT("QuestLogComponent: Quest started - %s"), *QuestId.ToString());
	
	// Auto-track if no quest is tracked
	if (TrackedQuestId.IsNone())
	{
		SetTrackedQuest(QuestId);
	}
}

void UQuestLogComponent::OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Log, TEXT("QuestLogComponent: Quest completed - %s"), *QuestId.ToString());
	
	// Clear tracked if this was tracked quest
	if (TrackedQuestId == QuestId)
	{
		ClearTrackedQuest();
	}
}

void UQuestLogComponent::OnQuestFailedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Warning, TEXT("QuestLogComponent: Quest failed - %s"), *QuestId.ToString());
	
	// Clear tracked if this was tracked quest
	if (TrackedQuestId == QuestId)
	{
		ClearTrackedQuest();
	}
}

void UQuestLogComponent::OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestLogComponent: Objective updated - %s.%s"), 
		*QuestId.ToString(), *ObjectiveId.ToString());
}

void UQuestLogComponent::OnObjectiveCompletedHandler(FName QuestId, FName ObjectiveId)
{
	UE_LOG(LogTemp, Log, TEXT("QuestLogComponent: Objective completed - %s.%s"), 
		*QuestId.ToString(), *ObjectiveId.ToString());
}
