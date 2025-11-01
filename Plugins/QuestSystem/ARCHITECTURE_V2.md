# Quest System - Improved Architecture v2.0

## �������� ������ ����������� (v1.0):

### ? ��� ���� �� ���:
1. **Monolithic Subsystem** - ���� ��� � ����� ������ (~1000+ �����)
2. **���������� ��������** - ��������� ������� �����
3. **Tight coupling** - �� �������� �� Subsystem
4. **������ �����������** - ��� ���������� ���������������
5. **������ ���������** - ���� ���� ��� �����

---

## ? ����� ����������� (v2.0):

### ��������:
- **Single Responsibility** - ������ ����� ������ ���� ����
- **Dependency Injection** - ���������� ����������
- **Async by Default** - ��� �������� �����������
- **Event-Driven** - ������ ����������� ����� events
- **Testable** - ������ ��������� ����� ����������� ��������

---

## ����� ���������:

### 1. **Core Layer** - ������� ����

```cpp
// ������ ������, ������� ������
QuestTypes.h     // Enums, ������� ����
QuestStructures.h     // Structs ��� ������
QuestObjective.h      // Objective definition
```

### 2. **Runtime Layer** - ������� ������

#### A. **Asset Management**
```cpp
UQuestAsset          // ������ ������ (PrimaryDataAsset)
UQuestAssetLoader  // Async �������� �������
UQuestAssetRegistry          // ������ ���� ������� � ����
```

**���������������**:
- �������� quest assets
- �����������
- Preloading

#### B. **Progress Management**
```cpp
UQuestProgressManager   // ���������� ���������� �������
  - StartQuest()
  - CompleteQuest()
  - UpdateObjective()
  - AdvancePhase()
```

**���������������**:
- ��������� ������� (Active/Completed/Failed)
- �������� objectives
- �������� ����� phases
- ��������� ������������

#### C. **Reward System**
```cpp
UQuestRewardSystem           // ������ ������
  - GrantReward()
  - ProcessQuestRewards()
  - GrantTags()
```

**���������������**:
- ������ items
- ������ currency
- ��������� stats
- Unlocking content

#### D. **Quest Subsystem** (�����������)
```cpp
UQuestSubsystem         // ���������� ������ - ������ �����������
  - GetProgressManager()
  - GetRewardSystem()
  - GetAssetLoader()
  - GetEventBus()
```

**���������������**:
- ������������� ������
- ����������� ����� ������������
- ��������� API ��� Blueprint
- Save/Load orchestration

#### E. **Event System**
```cpp
UQuestEventBus               // Event routing
  - EmitEvent()
  - Subscribe()
  - Unsubscribe()
```

**���������������**:
- Event dispatch
- Event filtering
- Subscription management

#### F. **Save System**
```cpp
UQuestSaveGame    // Persistent storage
UQuestSaveManager    // �����! Save/Load logic
```

**���������������**:
- Serialization
- Auto-save
- Save slots
- Cloud sync integration

---

### 3. **Component Layer** - Actor Components

```cpp
UQuestLogComponent           // Player quest tracking
  - GetActiveQuests()
  - TrackQuest()
  - GetQuestProgress()

UQuestGiverComponent         // NPC quest giving
  - GetAvailableQuests()
  - AcceptQuest()
  - TurnInQuest()
```

---

### 4. **Editor Layer** - Tools

```cpp
AssetTypeActions_QuestAsset  // Asset actions
QuestAssetDetails    // Property customization
QuestAssetFactory            // Asset creation
QuestEditorTools// �����! Validation, testing
```

---

### 5. **UI Layer** - Widgets

```cpp
UQuestJournalWidget       // Quest log UI
UQuestTrackerWidget      // �����! HUD tracker
UQuestDialogWidget      // �����! Quest accept/turn-in UI
```

---

## ���������� QuestSubsystem (v2.0):

```cpp
class QUESTSYSTEMRUNTIME_API UQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Initialization
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // === PUBLIC API (Blueprint-friendly) ===

    // Quest Management
    UFUNCTION(BlueprintCallable)
    void StartQuestAsync(FName QuestId, FOnQuestStarted OnStarted);

    UFUNCTION(BlueprintCallable)
    bool IsQuestActive(FName QuestId) const;

    UFUNCTION(BlueprintCallable)
    void UpdateObjectiveProgress(FName QuestId, FName ObjectiveId, int32 Amount);

    // === SUBSYSTEM ACCESS ===
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UQuestProgressManager* GetProgressManager() const { return ProgressManager; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UQuestRewardSystem* GetRewardSystem() const { return RewardSystem; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UQuestAssetLoader* GetAssetLoader() const { return AssetLoader; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UQuestEventBus* GetEventBus() const { return EventBus; }

protected:
    // Sub-systems (Single Responsibility)
    UPROPERTY()
    TObjectPtr<UQuestProgressManager> ProgressManager;

    UPROPERTY()
    TObjectPtr<UQuestRewardSystem> RewardSystem;

  UPROPERTY()
    TObjectPtr<UQuestAssetLoader> AssetLoader;

    UPROPERTY()
    TObjectPtr<UQuestEventBus> EventBus;

    UPROPERTY()
    TObjectPtr<UQuestSaveManager> SaveManager;

    void InitializeSubsystems();
};
```

**������ Subsystem ����� ~200 ����� ������ 1000+!**

---

## ������� ������������� ����� �����������:

### 1. Async Quest Start

```cpp
// Old (v1.0) - ����������, �����������
bool bSuccess = QuestSubsystem->StartQuest(QuestId);

// New (v2.0) - async, non-blocking
QuestSubsystem->StartQuestAsync(QuestId, FOnQuestStarted::CreateLambda([](bool bSuccess, UQuestAsset* Asset)
{
 if (bSuccess)
    {
        // Quest started!
    }
}));
```

### 2. Preloading Quests

```cpp
// Preload all tutorial quests at game start
TArray<FName> TutorialQuests = { "Q_Tutorial_1", "Q_Tutorial_2", "Q_Tutorial_3" };

QuestSubsystem->GetAssetLoader()->PreloadQuestAssets(TutorialQuests, FOnQuestAssetLoaded::CreateLambda([](UQuestAsset* Asset)
{
    UE_LOG(LogTemp, Log, TEXT("Tutorial quests preloaded"));
}));
```

### 3. ��������� ������������

```cpp
// ������ ����� ����������� ������ ������� ��������!

// Test Progress Manager
UQuestProgressManager* ProgressMgr = NewObject<UQuestProgressManager>();
ProgressMgr->StartQuest(QuestAsset);
check(ProgressMgr->IsQuestActive(QuestId));

// Test Reward System
UQuestRewardSystem* RewardSys = NewObject<UQuestRewardSystem>();
RewardSys->GrantReward(Reward, Player);
```

---

## Migration Guide (v1.0 ? v2.0):

### Breaking Changes:

| Old API (v1.0) | New API (v2.0) | Notes |
|----------------|----------------|-------|
| `StartQuest(QuestId)` | `StartQuestAsync(QuestId, Callback)` | Now async |
| `LoadQuestAsset(QuestId)` | `GetAssetLoader()->LoadQuestAssetAsync()` | Separated |
| `GrantReward()` | `GetRewardSystem()->GrantReward()` | Moved to RewardSystem |
| `SaveQuestProgress()` | `GetSaveManager()->SaveAsync()` | Moved to SaveManager |

### Compatibility Layer:

��� backward compatibility ����� �������� deprecated ������:

```cpp
// QuestSubsystem.h
UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "Use StartQuestAsync instead"))
bool StartQuest(FName QuestId);

// Implementation redirects to new API
bool UQuestSubsystem::StartQuest(FName QuestId)
{
    // Sync wrapper over async call (not recommended for production)
    bool bResult = false;
    StartQuestAsync(QuestId, FOnQuestStarted::CreateLambda([&bResult](bool bSuccess, UQuestAsset* Asset)
    {
        bResult = bSuccess;
    }));
    return bResult;
}
```

---

## Benefits:

### ? Maintainability
- ������ ���� < 300 �����
- ����� ����� ������ ���
- ������� git merge conflicts

### ? Testability
- ������ ������� - unit test
- Mock dependencies �����
- Integration tests �����

### ? Performance
- Async loading �� ���������
- Lazy initialization
- Parallel quest loading

### ? Extensibility
- �������� ����� ��� �������? ? QuestRewardSystem
- ����� ��� ����������? ? QuestSaveManager
- ��������� ���������? ? QuestProgressManager

### ? Team Workflow
- ��������� ������������� ����� �������� �����������
- ������ ���������� � git
- ����� code review

---

## Roadmap:

### v2.0.0 - Architecture Refactor
- [ ] Create QuestAssetLoader
- [ ] Create QuestProgressManager
- [ ] Create QuestRewardSystem
- [ ] Create QuestSaveManager
- [ ] Refactor QuestSubsystem (coordinator only)
- [ ] Migration guide
- [ ] Update documentation

### v2.1.0 - Advanced Features
- [ ] Quest chains system
- [ ] Dynamic quest generation
- [ ] Analytics integration
- [ ] A/B testing support

---

**Recommendation**: 
1. ������� v2.0 � ��������� �����
2. ����������� �������� (�� ������ v1.0)
3. �������� deprecated warnings
4. ����� 2-3 ������ ������� ������ API

---

**Next**: ������, ����� � ������ QuestProgressManager � QuestRewardSystem?
