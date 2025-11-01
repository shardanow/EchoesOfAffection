# Quest System v2.0 - ��������� �����������

## ? �������� ����������

### 1. QuestRewardSystem.cpp
```cpp
// ������: GrantTags ��������� TArray, � ��������� FGameplayTagContainer
// �������: �������������� TagContainer � TArray

void UQuestRewardSystem::ProcessQuestRewards(const FQuestRewardSet& Rewards, FName QuestId)
{
	for (const FQuestReward& Reward : Rewards.Rewards)
	{
		GrantReward(Reward, QuestId);
	}
	
	// FIX: Convert FGameplayTagContainer to TArray
	if (Rewards.GrantedTags.Num() > 0)
	{
		TArray<FGameplayTag> TagsArray;
		for (const FGameplayTag& Tag : Rewards.GrantedTags)
		{
			TagsArray.Add(Tag);
		}
		GrantTags(TagsArray);
	}
}

// FIX: GlobalQuestTags is TSet, not TArray
TArray<FName> UQuestRewardSystem::GetAllTags() const
{
	if (!SaveGame)
	{
		return TArray<FName>();
	}
	return SaveGame->GlobalQuestTags.Array();  // Convert TSet to TArray
}
```

### 2. QuestLogComponent.cpp
```cpp
// ������: ���������� ������ API Subsystem

// ��� ������ ����� ������������� �� ProgressManager:

void UQuestLogComponent::SubscribeToEvents()
{
	if (QuestSubsystem && QuestSubsystem->GetProgressManager())
	{
		auto* ProgressMgr = QuestSubsystem->GetProgressManager();
		ProgressMgr->OnQuestStarted.AddDynamic(this, &UQuestLogComponent::OnQuestStartedHandler);
		ProgressMgr->OnQuestCompleted.AddDynamic(this, &UQuestLogComponent::OnQuestCompletedHandler);
		ProgressMgr->OnQuestFailed.AddDynamic(this, &UQuestLogComponent::OnQuestFailedHandler);
		ProgressMgr->OnObjectiveUpdated.AddDynamic(this, &UQuestLogComponent::OnObjectiveUpdatedHandler);
		ProgressMgr->OnObjectiveCompleted.AddDynamic(this, &UQuestLogComponent::OnObjectiveCompletedHandler);
	}
}
```

### 3. QuestBlueprintLibrary.cpp
```cpp
// ������: ���������� ������ API (StartQuest ������ StartQuestAsync)

bool UQuestBlueprintLibrary::QuestStart(const UObject* WorldContextObject, FName QuestId)
{
	UQuestSubsystem* Subsystem = UQuestSubsystem::Get(WorldContextObject);
	if (!Subsystem)
	{
		return false;
	}
	
	// FIX: Use async version with empty callback
	FOnQuestStartedDelegate EmptyCallback;
	Subsystem->StartQuestAsync(QuestId, EmptyCallback);
	return true;  // Returns immediately, quest starts async
}
```

### 4. QuestSaveManager.cpp
```cpp
// ������: Dynamic delegate �� ������������ CreateLambda

void UQuestSaveManager::OnAutoSave()
{
	if (!CurrentSaveGame)
	{
		return;
	}
	
	// FIX: Don't use lambda with dynamic delegate
	FOnSaveCompleteCallback EmptyCallback;
	SaveQuestProgressAsync(CurrentSaveGame, EmptyCallback);
}
```

### 5. QuestSubsystem.h - �������� ����������� ������
```cpp
// Add to public section:

UFUNCTION(BlueprintCallable, Category = "Quest")
bool IsQuestCompleted(FName QuestId) const;

UFUNCTION(BlueprintCallable, Category = "Quest")
bool FailQuest(FName QuestId);

UFUNCTION(BlueprintCallable, Category = "Quest")
bool AbandonQuest(FName QuestId);

UFUNCTION(BlueprintCallable, Category = "Quest")
bool CompleteObjective(FName QuestId, FName ObjectiveId);

UFUNCTION(BlueprintCallable, Category = "Quest")
FName GetCurrentPhaseId(FName QuestId) const;
```

### 6. QuestSubsystem.cpp - ����������� ����������� ������
```cpp
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
```

---

## ?? ������� ����������

1. ? **QuestSubsystem.h** - �������� �������� StartQuest, FailQuest, AbandonQuest, etc.
2. ? **QuestSubsystem.cpp** - ����������� wrapper-������ ��� ProgressManager
3. ? **QuestRewardSystem.cpp** - Fix TagContainer ? TArray conversion
4. ? **QuestLogComponent.cpp** - Use ProgressManager events directly  
5. ? **QuestBlueprintLibrary.cpp** - Update to StartQuestAsync
6. ? **QuestSaveManager.cpp** - Remove lambda from dynamic delegate

---

## ?? ����� ����������� - ������ ����������������!

��� ��� ��������� ������ v2.0 ��������� �������������� � ����������� �������� ������������� ��� Blueprint API.
