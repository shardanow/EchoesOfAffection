# Сохранение и сброс данных Relationship System

## ?? Краткие ответы

### **1. Где сохраняются значения relationships?**

**В двух местах:**
1. ? **В памяти (Runtime)** - в `URelationshipSubsystem`
2. ? **На диске (SaveGame)** - через `URelationshipSaveGame`

---

### **2. Можно ли сбросить данные?**

**ДА**, есть **несколько способов**:

| Способ | Что сбрасывает | Когда использовать |
|--------|----------------|-------------------|
| **ClearAllRelationships()** | Все relationships | Полный сброс в игре |
| **RemoveRelationship()** | Один relationship | Удалить конкретный |
| **DeleteSave()** | Сохраненные данные | Удалить SaveGame |
| **Выход из игры** | Данные в памяти | Без сохранения |

---

## ?? Детальный анализ

### 1. **Runtime хранилище (в памяти)**

#### **Где находится:**
```cpp
// URelationshipSubsystem
// Plugins/RelationshipSystem/Source/RelationshipSystemCore/Private/Subsystems/RelationshipSubsystem.cpp

class URelationshipSubsystem : public UGameInstanceSubsystem
{
private:
    // Основное хранилище - TMap в памяти
    TMap<FRelationshipKey, FRelationshipData> Relationships;
};
```

#### **Что хранится:**
```cpp
struct FRelationshipData
{
    // Actor'ы
    TWeakObjectPtr<AActor> Subject;
    TWeakObjectPtr<AActor> Target;
    
    // Dimension значения
    TMap<FGameplayTag, FRelationshipDimensionValue> Dimensions;
    
    // State
    FRelationshipStateInstance State;
    
    // Traits
    TArray<FRelationshipTraitInstance> Traits;

    // Actions
    TMap<FGameplayTag, FRelationshipActionInstance> Actions;
    
  // Метаданные
    float LastUpdateTime;
};
```

#### **Жизненный цикл:**
```
URelationshipSubsystem::Initialize()
    ?
Relationships создаются во время игры
    ?
Хранятся в памяти пока работает игра
    ?
URelationshipSubsystem::Deinitialize()
    ?
Relationships.Empty() - очищается память
```

**Вывод:** Данные **НЕ сохраняются** между запусками игры автоматически ?

---

### 2. **Persistent хранилище (SaveGame)**

#### **Где находится:**
```cpp
// URelationshipSaveGame
// Plugins/RelationshipSystem/Source/RelationshipSystemCore/Public/SaveGame/RelationshipSaveGame.h

class URelationshipSaveGame : public USaveGame
{
public:
  // Версия формата сохранения
    int32 SaveVersion;

    // Сериализованные relationships
    TArray<FSerializedRelationship> SerializedRelationships;
    
    // Метаданные
    FDateTime SaveTimestamp;
    float SavedGameTime;
};
```

#### **Формат сохранения:**
```cpp
struct FSerializedRelationship
{
    FString SubjectActorName;        // Имя Subject Actor'а
    FString TargetActorName;         // Имя Target Actor'а
    TMap<FGameplayTag, float> DimensionValues;  // Dimension значения
 FGameplayTag CurrentState;       // Текущий State
    float TimeInState;        // Время в State
    TArray<FGameplayTag> ActiveTraits;          // Активные Traits
  TMap<FGameplayTag, int32> TraitStackCounts; // Stack counts
 TMap<FGameplayTag, int32> ActionUsageCounts;// Использования Actions
  TMap<FGameplayTag, float> ActionLastExecutionTimes;
    float LastUpdateTime;
};
```

#### **Файл на диске:**
```
Путь: {SavedGamesFolder}/RelationshipSave.sav

Пример Windows:
C:\Users\{UserName}\AppData\Local\{ProjectName}\Saved\SaveGames\RelationshipSave.sav
```

---

## ?? Как сохранять и загружать

### **Blueprint API:**

#### **Сохранить:**
```
Node: Save Relationships
Input:
  - Slot Name (опционально, default: "RelationshipSave")
Output:
  - Success (bool)
```

#### **Загрузить:**
```
Node: Load Relationships
Input:
  - Slot Name (опционально)
Output:
  - Success (bool)
```

#### **Проверить существование:**
```
Node: Does Save Exist
Input:
- Slot Name
Output:
  - Exists (bool)
```

#### **Удалить:**
```
Node: Delete Save
Input:
  - Slot Name
Output:
  - Success (bool)
```

---

### **C++ API:**

```cpp
// Сохранить relationships
bool bSaved = URelationshipSaveGameHelper::SaveRelationships(
GetWorld(),
    TEXT("RelationshipSave")  // Slot name (опционально)
);

// Загрузить relationships
bool bLoaded = URelationshipSaveGameHelper::LoadRelationships(
    GetWorld(),
    TEXT("RelationshipSave")
);

// Проверить существование сохранения
bool bExists = URelationshipSaveGameHelper::DoesSaveExist(TEXT("RelationshipSave"));

// Удалить сохранение
bool bDeleted = URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
```

---

## ??? Как сбросить данные

### **Вариант 1: Полный сброс в Runtime (в игре)**

```cpp
// Blueprint
GetGameInstance()->GetSubsystem(RelationshipSubsystem)->ClearAllRelationships()

// C++
UGameInstance* GameInstance = GetGameInstance();
URelationshipSubsystem* Subsystem = GameInstance->GetSubsystem<URelationshipSubsystem>();
Subsystem->ClearAllRelationships();
```

**Что делает:**
- ? Удаляет **ВСЕ** relationships из памяти
- ? Relationships.Empty() - очищает TMap
- ? **НЕ** удаляет SaveGame файл

**Когда использовать:**
- New Game
- Сброс прогресса в текущей сессии
- Тестирование

---

### **Вариант 2: Удалить один relationship**

```cpp
// Blueprint
RemoveRelationship(Subject, Target)

// C++
Subsystem->RemoveRelationship(Subject, Target);
```

**Что делает:**
- ? Удаляет **ОДИН** relationship между двумя Actor'ами
- ? Остальные relationships остаются

**Когда использовать:**
- NPC умер
- Relationship больше не нужен
- Очистка конкретного актора

---

### **Вариант 3: Удалить SaveGame файл**

```cpp
// Blueprint
DeleteSave("RelationshipSave")

// C++
URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
```

**Что делает:**
- ? Удаляет файл `.sav` с диска
- ? **НЕ** влияет на текущие relationships в памяти

**Когда использовать:**
- Полный сброс сохраненного прогресса
- Удаление старых save slot'ов
- Очистка перед New Game

---

### **Вариант 4: Не сохранять при выходе**

```cpp
// Просто выйдите из игры БЕЗ вызова SaveRelationships()
// Данные в памяти пропадут автоматически
```

**Что делает:**
- ? Relationships в памяти уничтожаются при выходе
- ? SaveGame файл остается без изменений

**Когда использовать:**
- Тестирование
- Откат после неудачных действий
- Временные эксперименты

---

### **Вариант 5: Комбинированный (полный сброс)**

```cpp
// Blueprint / C++
void ResetAllRelationshipData()
{
URelationshipSubsystem* Subsystem = GetSubsystem<URelationshipSubsystem>();
    
    // Шаг 1: Очистить runtime данные
    Subsystem->ClearAllRelationships();
    
    // Шаг 2: Удалить SaveGame
    URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
    
    UE_LOG(LogTemp, Log, TEXT("All relationship data reset!"));
}
```

**Когда использовать:**
- New Game (полный сброс)
- Очистка перед тестированием
- Reset to defaults

---

## ?? Сравнение методов сброса

| Метод | Runtime | SaveGame | Permanent | Use Case |
|-------|---------|----------|-----------|----------|
| **ClearAllRelationships()** | ? Очищает | ? Не трогает | ? Временный | Быстрый сброс |
| **RemoveRelationship()** | ? Удаляет один | ? Не трогает | ? Временный | Точечное удаление |
| **DeleteSave()** | ? Не трогает | ? Удаляет | ? Permanent | Сброс сохранений |
| **Выход без сохранения** | ? Автоматически | ? Не трогает | ? Откат | Отмена изменений |
| **Комбинированный** | ? Очищает | ? Удаляет | ? Permanent | New Game |

---

## ?? Проверка текущих данных

### **Узнать сколько relationships в памяти:**

```cpp
// Blueprint
int32 Count = GetSubsystem(RelationshipSubsystem)->GetRelationshipCount()

// C++
int32 Count = Subsystem->GetRelationshipCount();
UE_LOG(LogTemp, Log, TEXT("Active relationships: %d"), Count);
```

---

### **Проверить конкретный relationship:**

```cpp
// Blueprint
bool bExists = HasRelationship(Subject, Target)

// C++
bool bExists = Subsystem->HasRelationship(Subject, Target);
```

---

### **Получить все relationships одного актора:**

```cpp
// C++
TArray<FRelationshipData> SubjectRelationships = 
    Subsystem->GetRelationshipsAsSubject(PlayerCharacter);

UE_LOG(LogTemp, Log, TEXT("Player has %d relationships"), 
    SubjectRelationships.Num());
```

---

### **Проверить существование SaveGame:**

```cpp
// Blueprint
bool bSaveExists = DoesSaveExist("RelationshipSave")

// C++
bool bSaveExists = URelationshipSaveGameHelper::DoesSaveExist(TEXT("RelationshipSave"));
```

---

## ?? Автоматическое сохранение

### **Пример: Сохранение при смене уровня**

```cpp
// GameModeBase.cpp
void AMyGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    // Автосохранение relationships
    URelationshipSaveGameHelper::SaveRelationships(this);
}
```

---

### **Пример: Авто-загрузка при старте**

```cpp
// GameInstance.cpp
void UMyGameInstance::Init()
{
  Super::Init();
    
    // Автозагрузка если существует
  if (URelationshipSaveGameHelper::DoesSaveExist())
    {
   URelationshipSaveGameHelper::LoadRelationships(this);
    }
}
```

---

### **Пример: Периодическое сохранение**

```cpp
// PlayerController.cpp
void AMyPlayerController::BeginPlay()
{
  Super::BeginPlay();
    
    // Сохранение каждые 5 минут
    GetWorldTimerManager().SetTimer(
        AutoSaveTimerHandle,
    this,
        &AMyPlayerController::AutoSaveRelationships,
      300.0f,  // 5 минут
        true     // Повторять
  );
}

void AMyPlayerController::AutoSaveRelationships()
{
    URelationshipSaveGameHelper::SaveRelationships(this);
    UE_LOG(LogTemp, Log, TEXT("Relationships auto-saved"));
}
```

---

## ?? Debug / Тестирование

### **Console Commands (добавить в GameMode):**

```cpp
// GameMode.h
UCLASS()
class AMyGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    UFUNCTION(Exec)
    void RelDebugCount();
  
    UFUNCTION(Exec)
 void RelClearAll();
    
    UFUNCTION(Exec)
    void RelSave();
    
    UFUNCTION(Exec)
    void RelLoad();
    
    UFUNCTION(Exec)
    void RelDeleteSave();
};

// GameMode.cpp
void AMyGameMode::RelDebugCount()
{
    URelationshipSubsystem* Subsystem = GetGameInstance()->GetSubsystem<URelationshipSubsystem>();
    int32 Count = Subsystem->GetRelationshipCount();
    UE_LOG(LogTemp, Warning, TEXT("Active relationships: %d"), Count);
}

void AMyGameMode::RelClearAll()
{
    URelationshipSubsystem* Subsystem = GetGameInstance()->GetSubsystem<URelationshipSubsystem>();
    Subsystem->ClearAllRelationships();
    UE_LOG(LogTemp, Warning, TEXT("All relationships cleared!"));
}

void AMyGameMode::RelSave()
{
    bool bSuccess = URelationshipSaveGameHelper::SaveRelationships(this);
    UE_LOG(LogTemp, Warning, TEXT("Save %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
}

void AMyGameMode::RelLoad()
{
    bool bSuccess = URelationshipSaveGameHelper::LoadRelationships(this);
    UE_LOG(LogTemp, Warning, TEXT("Load %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
}

void AMyGameMode::RelDeleteSave()
{
    bool bSuccess = URelationshipSaveGameHelper::DeleteSave(TEXT("RelationshipSave"));
    UE_LOG(LogTemp, Warning, TEXT("Delete %s"), bSuccess ? TEXT("SUCCESS") : TEXT("FAILED"));
}
```

**Использование в консоли:**
```
RelDebugCount     ? Сколько relationships?
RelClearAll       ? Очистить всё
RelSave  ? Сохранить
RelLoad           ? Загрузить
RelDeleteSave     ? Удалить SaveGame
```

---

## ?? Где физически находятся файлы?

### **SaveGame файлы:**

```
Windows:
C:\Users\{UserName}\AppData\Local\{ProjectName}\Saved\SaveGames\RelationshipSave.sav

Linux:
~/.config/Epic/{ProjectName}/Saved/SaveGames/RelationshipSave.sav

Mac:
~/Library/Application Support/Epic/{ProjectName}/Saved/SaveGames/RelationshipSave.sav
```

### **В редакторе (PIE):**

```
{ProjectFolder}\Saved\SaveGames\RelationshipSave.sav
```

**Вручную удалить:** Просто удалите файл `.sav` ?

---

## ?? Важные замечания

### **1. Автосохранение НЕ включено по умолчанию**

```
Relationships создаются ? Хранятся в памяти ? БЕЗ автосохранения
```

**Нужно вручную вызывать:**
```cpp
URelationshipSaveGameHelper::SaveRelationships(this);
```

---

### **2. Actor names используются для идентификации**

```cpp
// При сохранении:
SerRel.SubjectActorName = Subject->GetName();  // "BP_Lianne_C_1"

// При загрузке:
AActor* Subject = FindActorByName(World, "BP_Lianne_C_1");
```

**Проблема:** Если имя Actor'а меняется или Actor не найден ? relationship НЕ восстановится ?

**Решение:** Используйте **стабильные** имена для NPC:
```cpp
// В Blueprint: SetActorLabel("Lianne")
// Или в C++: Actor->SetActorLabel(TEXT("Lianne"));
```

---

### **3. Relationships очищаются при Deinitialize**

```cpp
void URelationshipSubsystem::Deinitialize()
{
    ClearAllRelationships();  // ? Автоматически!
    RegisteredComponents.Empty();
}
```

**Вывод:** При выходе из игры данные **пропадают**, если не сохранить ?

---

## ? Рекомендации

### **1. Когда сохранять:**
- ? При переходе между уровнями
- ? Периодически (каждые 5-10 минут)
- ? При выходе из игры
- ? После важных событий

### **2. Когда загружать:**
- ? При старте новой сессии (`GameInstance::Init()`)
- ? После загрузки уровня

### **3. Когда сбрасывать:**
- ? New Game - полный сброс (Runtime + SaveGame)
- ? Death/Fail - сброс Runtime (откат к последнему Save)
- ? Debugging - ClearAllRelationships()

---

## ?? Итоговая схема

```
[Игра запущена]
       ?
LoadRelationships() ? Загрузить из SaveGame (если существует)
     ?
[Игра идет] ? Relationships в памяти (TMap)
       ?
ModifyDimensionValue(), AddTrait(), etc. ? Изменения в Runtime
   ?
SaveRelationships() ? Периодически сохранять
       ?
[Выход из игры]
       ?
Deinitialize() ? ClearAllRelationships() ? Память очищена
       ?
SaveGame файл остается на диске (если сохраняли)
```

---

## ?? Blueprint Example

### **Full Save/Load Flow:**

```
Event BeginPlay (GameInstance)
  ?
Does Save Exist? ? "RelationshipSave"
  ? YES
Load Relationships ? "RelationshipSave"
  ?
[Game Running...]
  ?
Event: Player Dies / Quits
  ?
Save Relationships ? "RelationshipSave"
  ?
End
```

### **New Game Flow:**

```
Event: "New Game" Button Clicked
  ?
Get Subsystem ? RelationshipSubsystem
  ?
ClearAllRelationships()
  ?
Delete Save ? "RelationshipSave"
  ?
Open Level ? "FirstLevel"
```

---

## ?? Связанные файлы

### **Исходный код:**
- `RelationshipSubsystem.h/cpp` - Runtime хранилище
- `RelationshipSaveGame.h/cpp` - SaveGame система
- `NPCRelationshipComponent.h/cpp` - Компоненты

### **DataAssets:**
- `DA_Dimension_*` - Настройки Dimensions (MinValue, MaxValue, DefaultValue)
- `DA_Profile_*` - InitialDimensions (начальные значения)

---

## ? Финальный ответ

### **Где сохраняются?**
1. **Runtime:** `URelationshipSubsystem::Relationships` (TMap в памяти)
2. **SaveGame:** `{SavedGames}/RelationshipSave.sav` (файл на диске)

### **Можно сбросить?**
? **ДА!** Несколько способов:

| Задача | Метод | Код |
|--------|-------|-----|
| **Сброс всех в игре** | ClearAllRelationships() | `Subsystem->ClearAllRelationships()` |
| **Удалить один** | RemoveRelationship() | `Subsystem->RemoveRelationship(Subject, Target)` |
| **Удалить SaveGame** | DeleteSave() | `URelationshipSaveGameHelper::DeleteSave("RelationshipSave")` |
| **Полный сброс** | Clear + Delete | `ClearAll()` + `DeleteSave()` |

### **Автосохранение?**
? **НЕТ** по умолчанию. Нужно вызывать вручную:
```cpp
URelationshipSaveGameHelper::SaveRelationships(this);
```

---

**Версия:** 1.0 | **Дата:** 2 ноября 2025 | **Автор:** Technical Documentation
