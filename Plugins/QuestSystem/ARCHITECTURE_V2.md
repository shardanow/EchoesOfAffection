# Quest System - Improved Architecture v2.0

## Проблемы старой архитектуры (v1.0):

### ? Что было не так:
1. **Monolithic Subsystem** - весь код в одном классе (~1000+ строк)
2. **Синхронная загрузка** - блокирует игровой поток
3. **Tight coupling** - всё завязано на Subsystem
4. **Сложно тестировать** - нет разделения ответственности
5. **Сложно расширять** - один файл для всего

---

## ? Новая архитектура (v2.0):

### Принципы:
- **Single Responsibility** - каждый класс делает одну вещь
- **Dependency Injection** - компоненты независимы
- **Async by Default** - вся загрузка асинхронная
- **Event-Driven** - слабая связанность через events
- **Testable** - каждый компонент можно тестировать отдельно

---

## Новая структура:

### 1. **Core Layer** - Базовые типы

```cpp
// Только данные, никакой логики
QuestTypes.h     // Enums, базовые типы
QuestStructures.h     // Structs для данных
QuestObjective.h      // Objective definition
```

### 2. **Runtime Layer** - Игровая логика

#### A. **Asset Management**
```cpp
UQuestAsset          // Данные квеста (PrimaryDataAsset)
UQuestAssetLoader  // Async загрузка квестов
UQuestAssetRegistry          // Реестр всех квестов в игре
```

**Ответственность**:
- Загрузка quest assets
- Кеширование
- Preloading

#### B. **Progress Management**
```cpp
UQuestProgressManager   // Управление прогрессом квестов
  - StartQuest()
  - CompleteQuest()
  - UpdateObjective()
  - AdvancePhase()
```

**Ответственность**:
- Состояние квестов (Active/Completed/Failed)
- Прогресс objectives
- Переходы между phases
- Валидация зависимостей

#### C. **Reward System**
```cpp
UQuestRewardSystem           // Выдача наград
  - GrantReward()
  - ProcessQuestRewards()
  - GrantTags()
```

**Ответственность**:
- Выдача items
- Выдача currency
- Изменение stats
- Unlocking content

#### D. **Quest Subsystem** (Координатор)
```cpp
UQuestSubsystem         // Упрощённая версия - только координация
  - GetProgressManager()
  - GetRewardSystem()
  - GetAssetLoader()
  - GetEventBus()
```

**Ответственность**:
- Инициализация систем
- Координация между компонентами
- Публичный API для Blueprint
- Save/Load orchestration

#### E. **Event System**
```cpp
UQuestEventBus               // Event routing
  - EmitEvent()
  - Subscribe()
  - Unsubscribe()
```

**Ответственность**:
- Event dispatch
- Event filtering
- Subscription management

#### F. **Save System**
```cpp
UQuestSaveGame    // Persistent storage
UQuestSaveManager    // НОВЫЙ! Save/Load logic
```

**Ответственность**:
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
QuestEditorTools// НОВЫЙ! Validation, testing
```

---

### 5. **UI Layer** - Widgets

```cpp
UQuestJournalWidget       // Quest log UI
UQuestTrackerWidget      // НОВЫЙ! HUD tracker
UQuestDialogWidget      // НОВЫЙ! Quest accept/turn-in UI
```

---

## Улучшенный QuestSubsystem (v2.0):

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

**Теперь Subsystem всего ~200 строк вместо 1000+!**

---

## Примеры использования новой архитектуры:

### 1. Async Quest Start

```cpp
// Old (v1.0) - синхронный, блокирующий
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

### 3. Модульное тестирование

```cpp
// Теперь можно тестировать каждую систему отдельно!

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

Для backward compatibility можно добавить deprecated методы:

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
- Каждый файл < 300 строк
- Легко найти нужный код
- Простые git merge conflicts

### ? Testability
- Каждая система - unit test
- Mock dependencies легко
- Integration tests проще

### ? Performance
- Async loading не блокирует
- Lazy initialization
- Parallel quest loading

### ? Extensibility
- Добавить новый тип награды? ? QuestRewardSystem
- Новый тип сохранения? ? QuestSaveManager
- Кастомная валидация? ? QuestProgressManager

### ? Team Workflow
- Несколько разработчиков могут работать параллельно
- Меньше конфликтов в git
- Проще code review

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
1. Создать v2.0 в отдельной ветке
2. Постепенная миграция (не ломать v1.0)
3. Добавить deprecated warnings
4. Через 2-3 релиза удалить старый API

---

**Next**: Хотите, чтобы я создал QuestProgressManager и QuestRewardSystem?
