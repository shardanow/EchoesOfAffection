// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/QuestJournalWidget.h"
#include "QuestSubsystem.h"
#include "QuestProgressManager.h"
#include "QuestAsset.h"
#include "Engine/World.h"

UQuestJournalWidget::UQuestJournalWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UQuestJournalWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	QuestSubsystem = GetQuestSubsystem();
	SubscribeToEvents();
	RefreshQuestList();
}

void UQuestJournalWidget::NativeDestruct()
{
	UnsubscribeFromEvents();
	Super::NativeDestruct();
}

UQuestSubsystem* UQuestJournalWidget::GetQuestSubsystem() const
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

void UQuestJournalWidget::SubscribeToEvents()
{
	if (QuestSubsystem && QuestSubsystem->GetProgressManager())
	{
		QuestSubsystem->GetProgressManager()->OnQuestStarted.AddDynamic(this, &UQuestJournalWidget::OnQuestStartedHandler);
		QuestSubsystem->GetProgressManager()->OnQuestCompleted.AddDynamic(this, &UQuestJournalWidget::OnQuestCompletedHandler);
		QuestSubsystem->GetProgressManager()->OnObjectiveUpdated.AddDynamic(this, &UQuestJournalWidget::OnObjectiveUpdatedHandler);
	}
}

void UQuestJournalWidget::UnsubscribeFromEvents()
{
	if (QuestSubsystem && QuestSubsystem->GetProgressManager())
	{
		QuestSubsystem->GetProgressManager()->OnQuestStarted.RemoveDynamic(this, &UQuestJournalWidget::OnQuestStartedHandler);
		QuestSubsystem->GetProgressManager()->OnQuestCompleted.RemoveDynamic(this, &UQuestJournalWidget::OnQuestCompletedHandler);
		QuestSubsystem->GetProgressManager()->OnObjectiveUpdated.RemoveDynamic(this, &UQuestJournalWidget::OnObjectiveUpdatedHandler);
	}
}

void UQuestJournalWidget::RefreshQuestList()
{
	if (!QuestSubsystem)
	{
		return;
	}
	
	CachedQuests = GetFilteredQuests();
	OnQuestListUpdated(CachedQuests);
}

void UQuestJournalWidget::SetSelectedQuest(FName QuestId)
{
	if (!QuestSubsystem)
	{
		return;
	}
	
	SelectedQuestId = QuestId;
	UQuestAsset* QuestAsset = QuestSubsystem->GetQuestAsset(QuestId);
	OnQuestSelected(QuestId, QuestAsset);
}

void UQuestJournalWidget::FilterByState(EQuestState State)
{
	FilterState = State;
	RefreshQuestList();
}

void UQuestJournalWidget::FilterByCategory(FGameplayTag CategoryTag)
{
	FilterCategory = CategoryTag;
	RefreshQuestList();
}

void UQuestJournalWidget::ClearFilters()
{
	FilterState = EQuestState::Active;
	FilterCategory = FGameplayTag();
	RefreshQuestList();
}

TArray<FQuestSaveData> UQuestJournalWidget::GetFilteredQuests() const
{
	TArray<FQuestSaveData> FilteredQuests;
	
	if (!QuestSubsystem)
	{
		return FilteredQuests;
	}
	
	TArray<FQuestSaveData> AllQuests;
	
	if (FilterState == EQuestState::Completed)
	{
		AllQuests = QuestSubsystem->GetCompletedQuests();
	}
	else
	{
		AllQuests = QuestSubsystem->GetActiveQuests();
	}
	
	if (FilterCategory.IsValid())
	{
		for (const FQuestSaveData& QuestData : AllQuests)
		{
			UQuestAsset* QuestAsset = QuestSubsystem->GetQuestAsset(QuestData.QuestId);
			if (QuestAsset && QuestAsset->Meta.Categories.HasTag(FilterCategory))
			{
				FilteredQuests.Add(QuestData);
			}
		}
	}
	else
	{
		FilteredQuests = AllQuests;
	}
	
	return FilteredQuests;
}

void UQuestJournalWidget::OnQuestStartedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	RefreshQuestList();
}

void UQuestJournalWidget::OnQuestCompletedHandler(FName QuestId, UQuestAsset* QuestAsset)
{
	RefreshQuestList();
}

void UQuestJournalWidget::OnObjectiveUpdatedHandler(FName QuestId, FName ObjectiveId)
{
	OnQuestProgressChanged(QuestId);
}
