// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuestTrackerWidget.h"
#include "QuestSubsystem.h"
#include "QuestProgressManager.h"
#include "QuestAsset.h"
#include "QuestAssetLoader.h"
#include "QuestLogComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "TimerManager.h"
#include "Engine/World.h"

UQuestTrackerWidget::UQuestTrackerWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UQuestTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: NativeConstruct called"));

	QuestSubsystem = GetQuestSubsystem();
	
	if (QuestSubsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Quest Subsystem found!"));
		
		SubscribeToEvents();

		// Auto-track first active quest if enabled
		if (bAutoTrackFirstQuest)
		{
			TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
			UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Found %d active quests"), ActiveQuests.Num());
			
			if (ActiveQuests.Num() > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Auto-tracking quest '%s'"), *ActiveQuests[0].QuestId.ToString());
				SetTrackedQuest(ActiveQuests[0].QuestId);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("QuestTrackerWidget: No active quests to track"));
			}
		}

		// Start update timer
		if (UpdateInterval > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				UpdateTimerHandle,
				this,
				&UQuestTrackerWidget::RefreshDisplay,
				UpdateInterval,
				true
			);
			UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Update timer started (%.2f seconds)"), UpdateInterval);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuestTrackerWidget: Quest Subsystem NOT FOUND!"));
	}

	RefreshDisplay();
}

void UQuestTrackerWidget::NativeDestruct()
{
	UnsubscribeFromEvents();

	if (UpdateTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}

	Super::NativeDestruct();
}

void UQuestTrackerWidget::SetTrackedQuest(FName QuestId)
{
	if (TrackedQuestId == QuestId)
	{
		return;
	}

	TrackedQuestId = QuestId;

	if (QuestSubsystem)
	{
		UQuestAsset* QuestAsset = QuestSubsystem->GetQuestAsset(QuestId);
		
		if (!QuestAsset)
		{
			// Asset not loaded yet - it will be loaded by subsystem
			UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Quest asset not loaded yet for '%s', waiting for load"), *QuestId.ToString());
			
			// Schedule a delayed refresh to give time for asset to load
			FTimerHandle DelayedRefreshTimer;
			GetWorld()->GetTimerManager().SetTimer(
				DelayedRefreshTimer,
				[this, QuestId]()
				{
					if (TrackedQuestId == QuestId)
					{
						UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Retrying refresh for '%s'"), *QuestId.ToString());
						RefreshDisplay();
					}
				},
				0.5f, // Retry after 0.5 seconds
				false
			);
		}
		else
		{
			OnTrackedQuestChanged(QuestId, QuestAsset);
		}
	}

	RefreshDisplay();
}

void UQuestTrackerWidget::ClearTrackedQuest()
{
	TrackedQuestId = NAME_None;
	RefreshDisplay();
}

UQuestSubsystem* UQuestTrackerWidget::GetQuestSubsystem() const
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

void UQuestTrackerWidget::SubscribeToEvents()
{
	if (QuestSubsystem && QuestSubsystem->GetProgressManager())
	{
		auto* ProgressMgr = QuestSubsystem->GetProgressManager();
		ProgressMgr->OnQuestStarted.AddDynamic(this, &UQuestTrackerWidget::OnQuestStartedHandler);
		ProgressMgr->OnObjectiveUpdated.AddDynamic(this, &UQuestTrackerWidget::OnObjectiveUpdatedHandler);
		ProgressMgr->OnQuestCompleted.AddDynamic(this, &UQuestTrackerWidget::OnQuestCompletedHandler);
		
		UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Subscribed to quest events"));
	}
}

void UQuestTrackerWidget::UnsubscribeFromEvents()
{
	if (QuestSubsystem && QuestSubsystem->GetProgressManager())
	{
		auto* ProgressMgr = QuestSubsystem->GetProgressManager();
		ProgressMgr->OnQuestStarted.RemoveDynamic(this, &UQuestTrackerWidget::OnQuestStartedHandler);
		ProgressMgr->OnObjectiveUpdated.RemoveDynamic(this, &UQuestTrackerWidget::OnObjectiveUpdatedHandler);
		ProgressMgr->OnQuestCompleted.RemoveDynamic(this, &UQuestTrackerWidget::OnQuestCompletedHandler);
		
		UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Unsubscribed from quest events"));
	}
}

void UQuestTrackerWidget::RefreshDisplay()
{
	if (TrackedQuestId.IsNone() || !QuestSubsystem)
	{
		// No quest tracked or subsystem not available
		if (TextQuestTitle)
		{
			TextQuestTitle->SetText(FText::FromString(TEXT("No Active Quests")));
		}
		if (TextObjective)
		{
			TextObjective->SetText(FText::GetEmpty());
		}
		if (ProgressBar)
		{
			ProgressBar->SetPercent(0.0f);
		}
		
		// Hide the entire widget
		SetVisibility(ESlateVisibility::Collapsed);
		
		UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: No active quest - widget hidden"));
		return;
	}

	// Show widget when quest is active
	SetVisibility(ESlateVisibility::Visible);

	UQuestAsset* QuestAsset = QuestSubsystem->GetQuestAsset(TrackedQuestId);
	if (!QuestAsset)
	{
		// Show loading state
		if (TextQuestTitle)
		{
			TextQuestTitle->SetText(FText::Format(
				FText::FromString(TEXT("Loading quest: {0}")),
				FText::FromName(TrackedQuestId)
			));
		}
		if (TextObjective)
		{
			TextObjective->SetText(FText::FromString(TEXT("Loading...")));
		}
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: Refreshing display for quest '%s'"), *TrackedQuestId.ToString());

	// Update quest title
	if (TextQuestTitle)
	{
		TextQuestTitle->SetText(QuestAsset->Title);
		UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: Set title to '%s'"), *QuestAsset->Title.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QuestTrackerWidget: TextQuestTitle widget is NULL! Check BindWidget."));
	}

	// Get current phase
	FName CurrentPhaseId = QuestSubsystem->GetCurrentPhaseId(TrackedQuestId);
	FQuestPhase Phase = QuestAsset->GetPhase(CurrentPhaseId);

	UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: Current phase '%s', %d objectives"), 
		*CurrentPhaseId.ToString(), Phase.Objectives.Num());

	// Collect ALL active (incomplete) objectives
	TArray<FString> ActiveObjectives;
	for (const FQuestObjective& Objective : Phase.Objectives)
	{
		if (Objective.bIsHidden)
		{
			continue; // Skip hidden objectives
		}

		int32 CurrentCount, TargetCount;
		bool bIsCompleted;
		QuestSubsystem->GetObjectiveProgress(TrackedQuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

		if (!bIsCompleted)
		{
			// Format objective text
			FString ObjectiveStr;
			if (TargetCount > 1)
			{
				ObjectiveStr = FString::Printf(TEXT("%s (%d/%d)"), 
					*Objective.Title.ToString(), 
					CurrentCount, 
					TargetCount);
			}
			else
			{
				ObjectiveStr = Objective.Title.ToString();
			}

			ActiveObjectives.Add(ObjectiveStr);
			
			UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: Objective '%s' - %d/%d"), 
				*Objective.Title.ToString(), CurrentCount, TargetCount);
		}
	}

	// Display objectives
	if (ActiveObjectives.Num() > 0)
	{
		// Join all objectives with newlines
		FString CombinedText = FString::Join(ActiveObjectives, TEXT("\n"));
		
		if (TextObjective)
		{
			TextObjective->SetText(FText::FromString(CombinedText));
			UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Showing %d active objectives"), ActiveObjectives.Num());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("QuestTrackerWidget: TextObjective widget is NULL! Check BindWidget."));
		}
	}
	else
	{
		// No active objectives (all completed or phase done)
		if (TextObjective)
		{
			TextObjective->SetText(FText::FromString(TEXT("All objectives complete!")));
		}
	}

	// Update progress bar
	if (bShowProgressBar)
	{
		UpdateProgressBar();
	}
}

void UQuestTrackerWidget::UpdateProgressBar()
{
	if (!ProgressBar || !QuestSubsystem)
	{
		return;
	}

	UQuestAsset* QuestAsset = QuestSubsystem->GetQuestAsset(TrackedQuestId);
	if (!QuestAsset)
	{
		return;
	}

	// Calculate completion percentage based on CURRENT PHASE only
	FName CurrentPhaseId = QuestSubsystem->GetCurrentPhaseId(TrackedQuestId);
	FQuestPhase Phase = QuestAsset->GetPhase(CurrentPhaseId);

	float TotalProgress = 0.0f;
	int32 TotalObjectives = 0;

	for (const FQuestObjective& Objective : Phase.Objectives)
	{
		if (!Objective.bIsOptional && !Objective.bIsHidden)
		{
			TotalObjectives++;

			int32 CurrentCount, TargetCount;
			bool bIsCompleted;
			QuestSubsystem->GetObjectiveProgress(TrackedQuestId, Objective.ObjectiveId, CurrentCount, TargetCount, bIsCompleted);

			// Calculate partial progress for this objective
			float ObjectiveProgress = 0.0f;
			if (TargetCount > 0)
			{
				ObjectiveProgress = FMath::Clamp((float)CurrentCount / (float)TargetCount, 0.0f, 1.0f);
			}
			else if (bIsCompleted)
			{
				ObjectiveProgress = 1.0f;
			}

			TotalProgress += ObjectiveProgress;

			UE_LOG(LogTemp, Verbose, TEXT("QuestTrackerWidget: Objective '%s' progress: %.2f%% (%d/%d)"),
				*Objective.ObjectiveId.ToString(),
				ObjectiveProgress * 100.0f,
				CurrentCount,
				TargetCount);
		}
	}

	// Calculate average progress across all objectives
	float Percent = (TotalObjectives > 0) ? TotalProgress / (float)TotalObjectives : 0.0f;
	
	UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Progress bar updated - %.2f%% (%d objectives)"),
		Percent * 100.0f,
		TotalObjectives);

	ProgressBar->SetPercent(Percent);

	OnQuestProgressUpdated(TrackedQuestId, Percent);
}

void UQuestTrackerWidget::OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId)
{
	if (QuestId == TrackedQuestId)
	{
		RefreshDisplay();
	}
}

void UQuestTrackerWidget::OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget::OnQuestStartedHandler - Quest '%s' started!"), *QuestId.ToString());
	
	// Auto-track new quest if enabled
	if (bAutoTrackFirstQuest && TrackedQuestId.IsNone())
	{
		UE_LOG(LogTemp, Log, TEXT("QuestTrackerWidget: Auto-tracking new quest '%s'"), *QuestId.ToString());
		SetTrackedQuest(QuestId);
	}
	
	// If this is our tracked quest, refresh display
	if (QuestId == TrackedQuestId)
	{
		RefreshDisplay();
	}
}

void UQuestTrackerWidget::OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	if (QuestId == TrackedQuestId)
	{
		OnTrackedQuestCompleted(QuestId);

		// Auto-switch to next quest if available
		if (bAutoTrackFirstQuest)
		{
			ClearTrackedQuest();

			TArray<FQuestSaveData> ActiveQuests = QuestSubsystem->GetActiveQuests();
			if (ActiveQuests.Num() > 0)
			{
				SetTrackedQuest(ActiveQuests[0].QuestId);
			}
		}
		else
		{
			ClearTrackedQuest();
		}
	}
}
